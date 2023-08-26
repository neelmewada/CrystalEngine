#include "EditorSystem.h"

namespace CE::Editor
{

	static TextureFormat GetTextureFormatFromCMImage(const CMImage& image)
	{
		if (image.GetNumChannels() == 1)
		{
			if (image.GetBitDepth() == 8)
				return TextureFormat::R8;
			else if (image.GetBitDepth() == 16)
				return TextureFormat::R16;
			else if (image.GetBitDepth() == 32)
				return TextureFormat::RFloat;
		}
		else if (image.GetNumChannels() == 2)
		{
			if (image.GetBitDepth() == 16)
				return TextureFormat::RG32;
			else if (image.GetBitDepth() == 8)
				return TextureFormat::RG16;
		}
		else if (image.GetNumChannels() == 3)
		{
			return TextureFormat::RGB24;
		}
		else if (image.GetNumChannels() == 4)
		{
			return TextureFormat::RGBA32;
		}

		return TextureFormat::None;
	}

	static TextureCompressionSettings GetPreferredSourceCompressionFormat(const CMImage& image, bool lowQuality = true)
	{
		if (image.GetNumChannels() == 1)
		{
			if (image.GetBitDepth() == 8)
				return TextureCompressionSettings::Grayscale;
		}
		else if (image.GetNumChannels() == 2)
		{
			if (image.GetBitDepth() == 8 || image.GetBitDepth() == 16)
				TextureCompressionSettings::NormalMap;
		}
		else if (image.GetNumChannels() == 3)
		{
			if (image.GetBitDepth() == 8)
				return TextureCompressionSettings::Default;
		}
		else if (image.GetNumChannels() == 4)
		{
			if (image.GetBitDepth() == 8)
			{
				return lowQuality ? TextureCompressionSettings::Default : TextureCompressionSettings::BC7;
			}
		}
		return TextureCompressionSettings::Default;
	}

	TextureImportJob::TextureImportJob(TextureAssetImporter* importer, const IO::Path& sourcePath, const IO::Path& outPath)
		: AssetImportJob(importer, sourcePath, outPath)
	{

	}

	void TextureImportJob::Process()
	{
		success = false;
		bool isGameAsset = false;

		if (outPath.IsEmpty())
			outPath = sourcePath.ReplaceExtension(".casset");
		if (!sourcePath.Exists())
			return;

		String sourceAssetRelativePath = "";

		if (IO::Path::IsSubDirectory(sourcePath, gProjectPath / "Game/Assets"))
		{
			sourceAssetRelativePath = IO::Path::GetRelative(sourcePath, gProjectPath).GetString().Replace({ '\\' }, '/');
			if (!sourceAssetRelativePath.StartsWith("/"))
				sourceAssetRelativePath = "/" + sourceAssetRelativePath;
		}

		String packageName = "";

		String assetName = outPath.GetFilename().RemoveExtension().GetString();
		if (!IsValidObjectName(assetName))
		{
			assetName = FixObjectName(assetName);
			outPath = outPath.GetParentPath() / (assetName + ".casset");
		}

		if (IO::Path::IsSubDirectory(outPath, gProjectPath / "Game/Assets"))
		{
			isGameAsset = true;
			packageName = IO::Path::GetRelative(outPath, gProjectPath).RemoveExtension().GetString().Replace({ '\\' }, '/');
			if (!packageName.StartsWith("/"))
				packageName = "/" + packageName;
		}
		else if (IO::Path::IsSubDirectory(outPath, PlatformDirectories::GetAppRootDir() / "Engine/Assets"))
		{
			packageName = IO::Path::GetRelative(outPath, PlatformDirectories::GetAppRootDir()).RemoveExtension().GetString().Replace({ '\\' }, '/');
			if (!packageName.StartsWith("/"))
				packageName = "/" + packageName;
		}
		else
		{
			packageName = assetName;
		}
		
		String extension = sourcePath.GetExtension().GetString();
		TextureAssetDefinition* textureAssetDef = GetAssetDefinition<TextureAssetDefinition>();
		if (textureAssetDef == nullptr)
			return;
		if (!textureAssetDef->GetSourceExtensions().Exists(extension))
			return;

		TextureSourceCompressionFormat sourceCompressionFormat = textureAssetDef->GetSourceCompressionFormatFromExtension(extension);
		if (sourceCompressionFormat == TextureSourceCompressionFormat::None)
		{
			//CE_LOG(Error, All, "Unsupported source extension {}", extension);
			return;
		}

		CMImage image = CMImage::LoadFromFile(sourcePath);
		if (!image.IsValid())
		{
			//CE_LOG(Error, All, "Failed to load image at {}.\nReason: {}", sourcePath, image.GetFailureReason());
			return;
		}

		Package* texturePackage = nullptr;
		Texture2D* texture = nullptr;

		if (outPath.Exists())
		{
			texturePackage = Package::LoadPackage(nullptr, outPath, LOAD_Full);
			if (texturePackage != nullptr && texturePackage->GetSubObjectCount() > 0)
			{
				for (auto [uuid, subobject] : texturePackage->GetSubObjectMap())
				{
					if (subobject != nullptr && subobject->IsOfType<Texture2D>())
					{
						texture = (Texture2D*)subobject;
						break;
					}
				}
			}
		}

		if (texturePackage == nullptr)
			texturePackage = CreateObject<Package>(nullptr, packageName);

		if (texture == nullptr)
			texture = CreateObject<Texture2D>(texturePackage, assetName);
		else if (texture->GetName() != assetName)
			texture->SetName(assetName);
		
		defer(
			image.Free();
			texturePackage->RequestDestroy();
		);

		texture->width = image.GetWidth();
		texture->height = image.GetHeight();
		texture->depth = 1;
		texture->filter = TextureFilter::Linear;

		auto imageFormat = image.GetFormat();
		auto imageBitDepth = image.GetBitDepth();
		CMImageSourceFormat inputSourceFormat = image.GetSourceFormat();
		CMImageSourceFormat outputSourceFormat = image.GetSourceFormat();
		texture->compression = TextureCompressionSettings::Default;
		
		switch (imageFormat)
		{
		case CMImageFormat::R:
			if (imageBitDepth == 8 && compressByDefault)
			{
				outputSourceFormat = CMImageSourceFormat::BC4;
				texture->compression = TextureCompressionSettings::Grayscale;
				sourceCompressionFormat = TextureSourceCompressionFormat::BC4;
				texture->pixelFormat = TextureFormat::BC4;
			}
			else
			{
				if (imageBitDepth == 8)
				{
					texture->compression = TextureCompressionSettings::Grayscale;
					texture->pixelFormat = TextureFormat::R8;
				}
				else if (imageBitDepth == 16)
				{
					texture->compression = TextureCompressionSettings::Grayscale;
					texture->pixelFormat = TextureFormat::R16;
				}
				else return;
			}
			break;
		case CMImageFormat::RG:
			if (imageBitDepth == 8 && compressByDefault)
			{
				outputSourceFormat = CMImageSourceFormat::BC5;
				texture->compression = TextureCompressionSettings::GrayscaleAlpha;
				sourceCompressionFormat = TextureSourceCompressionFormat::BC5;
				texture->pixelFormat = TextureFormat::BC5;
			}
			else
			{
				if (imageBitDepth == 8)
				{
					texture->compression = TextureCompressionSettings::GrayscaleAlpha;
					texture->pixelFormat = TextureFormat::RG16;
				}
				else if (imageBitDepth == 16)
				{
					texture->compression = TextureCompressionSettings::GrayscaleAlpha;
					texture->pixelFormat = TextureFormat::RG32;
				}
				else return;
			}
			break;
		case CMImageFormat::RGB:
			if ((sourceCompressionFormat == TextureSourceCompressionFormat::HDR ||
				sourceCompressionFormat == TextureSourceCompressionFormat::EXR) && imageBitDepth == 16) // HDR image
			{
				outputSourceFormat = compressByDefault ? CMImageSourceFormat::BC6H :
					(sourceCompressionFormat == TextureSourceCompressionFormat::HDR ? CMImageSourceFormat::HDR : CMImageSourceFormat::EXR);
				texture->compression = compressByDefault ? TextureCompressionSettings::HDR : TextureCompressionSettings::HDRUncompressed;
				sourceCompressionFormat = compressByDefault ? TextureSourceCompressionFormat::BC6H : TextureSourceCompressionFormat::HDR;
				texture->pixelFormat = compressByDefault ? TextureFormat::BC6H : TextureFormat::RGBAHalf;
			}
			else if (imageBitDepth == 8 && compressByDefault)
			{
				outputSourceFormat = highQualityCompression ? CMImageSourceFormat::BC7 : CMImageSourceFormat::BC1;
				texture->compression = highQualityCompression ? TextureCompressionSettings::BC7 : TextureCompressionSettings::Default;
				sourceCompressionFormat = highQualityCompression ? TextureSourceCompressionFormat::BC7 : TextureSourceCompressionFormat::BC1;
				texture->pixelFormat = highQualityCompression ? TextureFormat::BC7 : TextureFormat::BC1;
			}
			else if (imageBitDepth == 8)
			{
				texture->compression = TextureCompressionSettings::ColorUncompressed;
				texture->pixelFormat = TextureFormat::RGB24;
			}
			else return;

			break;
		case CMImageFormat::RGBA:
			if (imageBitDepth == 8 && compressByDefault)
			{
				outputSourceFormat = highQualityCompression ? CMImageSourceFormat::BC7 : CMImageSourceFormat::BC3;
				texture->compression = highQualityCompression ? TextureCompressionSettings::BC7 : TextureCompressionSettings::Default;
				sourceCompressionFormat = highQualityCompression ? TextureSourceCompressionFormat::BC7 : TextureSourceCompressionFormat::BC3;
				texture->pixelFormat = highQualityCompression ? TextureFormat::BC7 : TextureFormat::BC3;
			}
			else
			{
				if (imageBitDepth == 8)
					texture->pixelFormat = TextureFormat::RGBA32;
				else if (imageBitDepth == 16)
					texture->pixelFormat = TextureFormat::RGBAHalf;
				else if (imageBitDepth == 32)
					texture->pixelFormat = TextureFormat::RGBAFloat;
				else
					return;
			}
			break;
		default:

			break;
		}

		if (texture->pixelFormat == TextureFormat::None)
		{
			//CE_LOG(Error, All, "Invalid number of channels: {}", image.GetNumChannels());
			return;
		}

		// BCn compression
		if (TextureSourceCompressionFormatIsBCn(sourceCompressionFormat))
		{
			MemoryStream memStream = MemoryStream(1024);
			memStream.SetBinaryMode(true);
			memStream.SetAutoResizeIncrement(1024);
			bool result = CMImage::EncodeToBCn(image, &memStream, outputSourceFormat, compressionQuality);
			if (!result)
			{
				return;
			}
			memStream.Seek(0, SeekMode::Begin);
			texture->source.rawData.LoadData(memStream.GetRawDataPtr(), memStream.GetLength());
		}
		else
		{
			FileStream fileStream = FileStream(sourcePath, Stream::Permissions::ReadOnly);
			fileStream.SetBinaryMode(true);
			texture->source.rawData.LoadData(&fileStream);
		}

		texture->source.sourcePixelFormat = texture->pixelFormat;
		texture->source.sourceCompression = sourceCompressionFormat;
		texture->addressMode = TextureAddressMode::Wrap;

		FieldType* sourceAssetPathField = texture->GetClass()->FindFieldWithName("sourceAssetRelativePath", TYPEID(String));
		if (sourceAssetPathField != nullptr)
		{
			sourceAssetPathField->ForceSetFieldValue<String>(texture, sourceAssetRelativePath);
		}

		SavePackageResult result = Package::SavePackage(texturePackage, texture, outPath);
		if (result != SavePackageResult::Success)
		{
			return;
		}

		outPackagePath = packageName;
		success = true;
	}

	bool TextureAssetImporter::GenerateThumbnail(const Name& packagePath, BinaryBlob& outThumbnailPNG)
	{
		CE_LOG(Info, All, "Generating thumbnail: {}", packagePath);
		return false;
	}

	Array<AssetImportJob*> TextureAssetImporter::CreateImportJobs(const Array<IO::Path>& sourcePaths, const Array<IO::Path>& productPaths)
	{
		Array<AssetImportJob*> result{};

		for (int i = 0; i < sourcePaths.GetSize(); i++)
		{
			IO::Path productPath = {};
			if (i < productPaths.GetSize())
				productPath = productPaths[i];

			auto job = new TextureImportJob(this, sourcePaths[i], productPath);
			job->compressByDefault = compressByDefault;
			job->highQualityCompression = highQualityCompression;
			job->compressionQuality = compressionQuality;

			result.Add(job);
		}

		return result;
	}

} // namespace CE::Editor
