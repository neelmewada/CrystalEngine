
#include "System.h"

namespace CE
{
	TextureAsset::TextureAsset()
		: Asset("Texture")
	{

	}

	TextureAsset::~TextureAsset()
	{

	}

	ResourceObject* TextureAsset::InstantiateResource()
	{
		return new Texture(this);
	}

	TextureFileFormat TextureAsset::GetFileFormat()
	{
		if (assetExtension.IsEmpty())
			return TextureFileFormat::Undefined;
		if (!assetExtension.StartsWith("."))
			assetExtension = "." + assetExtension;

		if (assetExtension == ".png")
			return TextureFileFormat::PNG;
		if (assetExtension == ".jpg" || assetExtension == ".jpeg")
			return TextureFileFormat::JPG;
		if (assetExtension == ".tiff" || assetExtension == ".tif")
			return TextureFileFormat::TIFF;

		return TextureFileFormat::Undefined;
	}

} // namespace CE

