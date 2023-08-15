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

		// Store in BCn format if possible
		MemoryStream memStream = MemoryStream(1024);
		memStream.SetBinaryMode(true);
		memStream.SetAutoResizeIncrement(512);

		texture->pixelFormat = GetTextureFormatFromCMImage(image);
		if (texture->pixelFormat == TextureFormat::None)
		{
			//CE_LOG(Error, All, "Invalid number of channels: {}", image.GetNumChannels());
			return;
		}

		// Save as original (only PNG for now)
		FileStream fileData = FileStream(sourcePath, Stream::Permissions::ReadOnly);
		texture->source.rawData.LoadData(&fileData);
		texture->source.sourcePixelFormat = texture->pixelFormat;
		texture->source.sourceCompression = sourceCompressionFormat;
		texture->compression = GetPreferredSourceCompressionFormat(image);
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

	Array<AssetImportJob*> TextureAssetImporter::CreateImportJobs(const Array<IO::Path>& sourcePaths, const Array<IO::Path>& productPaths)
	{
		Array<AssetImportJob*> result{};

		for (int i = 0; i < sourcePaths.GetSize(); i++)
		{
			IO::Path productPath = {};
			if (i < productPaths.GetSize() - 1)
				productPath = productPaths[i];

			result.Add(new TextureImportJob(this, sourcePaths[i], productPath));
		}

		return result;
	}

} // namespace CE::Editor
