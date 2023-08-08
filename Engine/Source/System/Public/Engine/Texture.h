#pragma once

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
	namespace Editor { class TextureAssetImporter; }
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
			friend class CE::Editor::TextureAssetImporter;
#endif

			FIELD()
			BinaryBlob rawData{};

			FIELD()
			TextureSourceFormat sourceFormat = TextureSourceFormat::PNG;

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

		FIELD(ReadOnly)
		Private::TextureSource source{};

		FIELD()
		TextureType type = TextureType::Default;

		FIELD()
		TextureFilter filter = TextureFilter::Linear;

		FIELD()
		u32 width = 0;

		FIELD()
		u32 height = 0;

		FIELD()
		u32 depth = 1;

	};
    
} // namespace CE

#include "Texture.rtti.h"
