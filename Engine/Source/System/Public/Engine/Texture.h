#pragma once

#include "TextureDefines.h"

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
	namespace Editor { class TextureImportJob; }
#endif

	namespace Private
	{
		STRUCT()
		struct SYSTEM_API TextureSource
		{
			CE_STRUCT(TextureSource)
		public:

			TextureSource();

			bool IsValid();

		private:
			
			// Automatically called when struct is destroyed
			void Release();

#if PAL_TRAIT_BUILD_EDITOR
			friend class CE::Editor::TextureImportJob;
#endif

			FIELD()
			BinaryBlob rawData{};

			FIELD()
			CE::TextureFormat sourcePixelFormat = CE::TextureFormat::None;

			FIELD()
			TextureSourceCompressionFormat sourceCompression = TextureSourceCompressionFormat::PNG;

		};
	}

	CLASS(Abstract)
	class SYSTEM_API Texture : public Asset
	{
		CE_CLASS(Texture, Asset)
	public:

		Texture();
		virtual ~Texture();

	protected:

		FIELD(Hidden)
		Private::TextureSource rawData{};

		FIELD()
		CE::TextureFormat pixelFormat = CE::TextureFormat::None;

		FIELD()
		TextureCompressionSettings compression = TextureCompressionSettings::Default;

		FIELD()
		RHI::FilterMode filter = RHI::FilterMode::Linear;

		FIELD()
		TextureAddressMode addressMode = TextureAddressMode::Wrap;

		FIELD()
		u8 anisotropy = 0;

		FIELD()
		u32 width = 0;

		FIELD()
		u32 height = 0;

		FIELD()
		u32 depth = 1;

	};
    
} // namespace CE

#include "Texture.rtti.h"
