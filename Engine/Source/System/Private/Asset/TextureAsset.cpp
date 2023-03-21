
#include "Asset/TextureAsset.h"

namespace CE
{
	TextureAsset::TextureAsset()
		: Asset("Texture")
	{

	}

	TextureAsset::~TextureAsset()
	{

	}

	AssetType TextureAsset::GetAssetType()
	{
		return AssetType::Texture;
	}

} // namespace CE

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, TextureAsset)
