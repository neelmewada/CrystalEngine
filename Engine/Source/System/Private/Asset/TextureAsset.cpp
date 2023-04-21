
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

} // namespace CE

