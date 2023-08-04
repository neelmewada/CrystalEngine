#include "EditorSystem.h"

namespace CE::Editor
{

	Name TextureAssetImporter::ImportSourceAsset(const IO::Path& sourceAssetPath, const IO::Path& productAssetPath, bool linkSourceAsset)
	{
		IO::Path outPath = productAssetPath;
		if (outPath.IsEmpty())
			outPath = sourceAssetPath.ReplaceExtension(".casset");
		if (!sourceAssetPath.Exists())
			return Name();

		String assetName = outPath.GetFilename().RemoveExtension().GetString();
		if (!IsValidObjectName(assetName))
		{
			assetName = FixObjectName(assetName);
			outPath = outPath.GetParentPath() / (assetName + ".casset");
		}

		String outPackageNameString = outPath.GetString().Replace({ '\\' }, '/');

		String packageName = "";

		if (outPackageNameString.Contains("Game/Assets"))
		{
			Array<String> split = {};
			outPackageNameString.Split({ "Game/Assets" }, split);
			packageName = String() + "/Game" + (split.GetLast().StartsWith("/") ? "" : "/") + split.GetLast();
		}
		else if (outPackageNameString.Contains("Engine/Assets"))
		{
			Array<String> split = {};
			outPackageNameString.Split({ "Engine/Assets" }, split);
			packageName = String() + "/Engine" + (split.GetLast().StartsWith("/") ? "" : "/") + split.GetLast();
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
		
		Package* texturePackage = CreateObject<Package>(nullptr, packageName);
		Texture2D* texture = CreateObject<Texture2D>(texturePackage, assetName);
		
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

		FileStream fileData = FileStream(sourceAssetPath, Stream::Permissions::ReadOnly);
		texture->source.rawData.LoadData(&fileData);

		if (image.GetNumChannels() == 1)
			texture->format = TextureFormat::RFloat;
		else if (image.GetNumChannels() == 3)
			texture->format = TextureFormat::RGBA32;
		else
			texture->format = TextureFormat::RGBA32;

		SavePackageResult result = Package::SavePackage(texturePackage, texture, outPath);
		if (result != SavePackageResult::Success)
		{
			failed = true;
			return Name();
		}

		return packageName;
	}

} // namespace CE::Editor
