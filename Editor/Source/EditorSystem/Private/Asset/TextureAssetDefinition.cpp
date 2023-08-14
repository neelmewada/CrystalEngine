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
			".png", ".jpg", ".jpeg"
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

	TextureSourceCompressionFormat TextureAssetDefinition::GetSourceCompressionFormatFromExtension(const String& sourceExtension)
	{
		if (sourceExtension.Contains("png"))
			return TextureSourceCompressionFormat::PNG;
		else if (sourceExtension.Contains("jpg") || sourceExtension.Contains("jpeg"))
			return TextureSourceCompressionFormat::JPG;
		else if (sourceExtension.Contains("hdr"))
			return TextureSourceCompressionFormat::HDR;
		else if (sourceExtension.Contains("exr"))
			return TextureSourceCompressionFormat::EXR;
		return TextureSourceCompressionFormat::None;
	}

} // namespace CE
