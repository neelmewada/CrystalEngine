#pragma once

#include "TextureDefines.h"

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
	namespace Editor { class TextureAssetImportJob; }
#endif

	class Texture;
	
	CLASS(Abstract)
	class SYSTEM_API Texture : public Asset
	{
		CE_CLASS(Texture, Asset)
	public:

		Texture();
		virtual ~Texture();

	protected:

		FIELD()
		BinaryBlob source{};

		FIELD()
		CE::TextureFormat pixelFormat = CE::TextureFormat::None;

		FIELD()
		TextureCompressionSettings compression = TextureCompressionSettings::Default;

		FIELD()
		RHI::FilterMode filter = RHI::FilterMode::Linear;

		FIELD()
		TextureAddressMode addressModeU = TextureAddressMode::Wrap;

		FIELD()
		TextureAddressMode addressModeV = TextureAddressMode::Wrap;

		FIELD()
		TextureDimension dimension = TextureDimension::None;

		FIELD()
		u8 anisoLevel = 0;

		FIELD()
		u32 width = 0;

		FIELD()
		u32 height = 0;

		FIELD()
		u32 mipLevels = 1;
	};
    
} // namespace CE

#include "Texture.rtti.h"
