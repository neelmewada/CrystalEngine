#include "System.h"

namespace CE::Editor
{
	TextureAssetDefinition::TextureAssetDefinition()
	{
		
	}

	TextureAssetDefinition::~TextureAssetDefinition()
	{
		
	}

	const Array<String>& TextureAssetDefinition::GetSourceExtensions()
	{
		static Array<String> extensions = {
			".png", ".jpg"
		};
		return extensions;
	}

	ClassType* TextureAssetDefinition::GetAssetClass()
	{
		return GetStaticClass<Texture2D>();
	}

	ClassType* TextureAssetDefinition::GetAssetClass(const String& extension)
	{
		return GetAssetClass();
	}

	SubClassType<AssetImporter> TextureAssetDefinition::GetAssetImporterClass()
	{
		return GetStaticClass<TextureAssetImporter>();
	}

	TextureSourceFormat TextureAssetDefinition::GetSourceFormatFromExtension(const String& sourceExtension)
	{
		if (sourceExtension.Contains("png"))
			return TextureSourceFormat::PNG;
		else if (sourceExtension.Contains("jpg") || sourceExtension.Contains("jpeg"))
			return TextureSourceFormat::JPG;
		return TextureSourceFormat::Unsupported;
	}

} // namespace CE
