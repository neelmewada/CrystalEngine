#pragma once

#include "CoreMedia.h"
#include "GameFramework/ResourceObject.h"

namespace CE
{
	class RHITexture;
	class TextureAsset;

	CLASS()
	class SYSTEM_API Texture : public ResourceObject
	{
		CE_CLASS(Texture, CE::ResourceObject)
	protected:
		Texture();
	public:
		Texture(TextureAsset* textureAsset);
		virtual ~Texture();

		bool IsValid();

	private:
		bool isValid = false;
	};

} // namespace CE

#include "Texture.rtti.h"
