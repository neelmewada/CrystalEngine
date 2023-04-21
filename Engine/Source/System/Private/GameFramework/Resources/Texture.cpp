
#include "System.h"

namespace CE
{
	Texture::Texture() : ResourceObject("Texture")
	{
		
	}

	Texture::Texture(TextureAsset* textureAsset) : ResourceObject(textureAsset != nullptr ? textureAsset->GetName() : "Texture")
	{
		isValid = false;
		if (textureAsset == nullptr)
			return;

	}

	Texture::~Texture()
	{

	}

	bool Texture::IsValid()
	{
		return isValid;
	}

} // namespace CE

