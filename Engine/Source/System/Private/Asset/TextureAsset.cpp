
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

} // namespace CE

CE_RTTI_ENUM_IMPL(SYSTEM_API, CE, TextureDataType)

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, TextureAsset)
