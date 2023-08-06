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

		FileStream fileData = FileStream(sourceAssetPath, Stream::Permissions::ReadOnly);
		texture->source.rawData.LoadData(&fileData);
		
		if (image.GetNumChannels() == 1)
			texture->format = TextureFormat::RFloat;
		else if (image.GetNumChannels() == 3)
			texture->format = TextureFormat::RGBA32;
		else
			texture->format = TextureFormat::RGBA32;

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
