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

	Name TextureAssetImporter::ImportSourceAsset(const IO::Path& sourceAssetPath, const IO::Path& productAssetPath, bool linkSourceAsset)
	{
		IO::Path outPath = productAssetPath;
		if (outPath.IsEmpty())
			outPath = sourceAssetPath.ReplaceExtension(".casset");
		if (!sourceAssetPath.Exists())
			return Name();

		auto assetRegistry = AssetRegistry::Get();

		String sourceAssetRelativePath = "";

		if (IO::Path::IsSubDirectory(sourceAssetPath, gProjectPath / "Game/Assets"))
		{
			sourceAssetRelativePath = IO::Path::GetRelative(sourceAssetPath, gProjectPath).GetString().Replace({ '\\' }, '/');
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
			packageName = IO::Path::GetRelative(outPath, gProjectPath).RemoveExtension().GetString().Replace({'\\'}, '/');
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

		String extension = sourceAssetPath.GetExtension().GetString();
		TextureAssetDefinition* textureAssetDef = GetAssetDefinition<TextureAssetDefinition>();
		if (textureAssetDef == nullptr)
			return Name();
		if (!textureAssetDef->GetSourceExtensions().Exists(extension))
			return Name();

		TextureSourceFormat sourceFormat = textureAssetDef->GetSourceFormatFromExtension(extension);
		if (sourceFormat == TextureSourceFormat::Unsupported)
		{
			CE_LOG(Error, All, "Unsupported source extension {}", extension);
			return Name();
		}
		
		CMImage image = CMImage::LoadFromFile(sourceAssetPath);
		if (!image.IsValid())
		{
			CE_LOG(Error, All, "Failed to load image at {}.\nReason: {}", sourceAssetPath, image.GetFailureReason());
			return Name();
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
		
		bool failed = false;
		
		defer (
			texturePackage->RequestDestroy();
			if (failed)
			{
				CE_LOG(Error, All, "Failed to import texture: {}", sourceAssetPath);
			}
		);

		texture->width = image.GetWidth();
		texture->height = image.GetHeight();
		texture->depth = 1;
		texture->type = textureType;
		texture->filter = TextureFilter::Linear;

		// Store in BCn format if possible
		MemoryStream memStream = MemoryStream(1024);
		memStream.SetBinaryMode(true);
		memStream.SetAutoResizeIncrement(512);

		// Encode image
		CMImageSourceFormat outputFormat = CMImageSourceFormat::Undefined;
		if (CMImage::EncodeToBCn(image, &memStream, outputFormat))
		{
			texture->source.sourceFormat = TextureSourceFormat::Unsupported;
			// BC4 or BC7 encode success
			switch (outputFormat)
			{
			case CE::CMImageSourceFormat::PNG:
				texture->source.sourceFormat = TextureSourceFormat::PNG;
				texture->format = GetTextureFormatFromCMImage(image);
				break;
			case CE::CMImageSourceFormat::BC4:
				texture->source.sourceFormat = TextureSourceFormat::BC4;
				texture->format = TextureFormat::BC4;
				break;
			case CE::CMImageSourceFormat::BC6H:
				texture->source.sourceFormat = TextureSourceFormat::BC6H;
				texture->format = TextureFormat::BC6H;
				break;
			case CE::CMImageSourceFormat::BC7:
				texture->source.sourceFormat = TextureSourceFormat::BC7;
				texture->format = TextureFormat::BC7_RGBA;
				break;
			}
			if (texture->source.sourceFormat == TextureSourceFormat::Unsupported ||
				texture->format == TextureFormat::None)
			{
				failed = true;
				CE_LOG(Error, All, "Invalid output format");
				return Name();
			}
			memStream.Seek(0);
			texture->source.rawData.LoadData(&memStream);
		}
		else
		{
			texture->format = GetTextureFormatFromCMImage(image);
			if (texture->format == TextureFormat::None)
			{
				failed = true;
				CE_LOG(Error, All, "Invalid number of channels: {}", image.GetNumChannels());
				return Name();
			}

			// Save as original (only PNG for now)
			FileStream fileData = FileStream(sourceAssetPath, Stream::Permissions::ReadOnly);
			texture->source.rawData.LoadData(&fileData);
			texture->source.sourceFormat = sourceFormat;
		}

		FieldType* sourceAssetPathField = texture->GetClass()->FindFieldWithName("sourceAssetRelativePath", TYPEID(String));
		if (sourceAssetPathField != nullptr)
		{
			sourceAssetPathField->ForceSetFieldValue<String>(texture, sourceAssetRelativePath);
		}

		SavePackageResult result = Package::SavePackage(texturePackage, texture, outPath);
		if (result != SavePackageResult::Success)
		{
			failed = true;
			return Name();
		}

		return packageName;
	}

} // namespace CE::Editor
