#pragma once

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
	namespace Editor { class TextureAssetImportJob; }
#endif

	CLASS()
	class SYSTEM_API Texture2D : public CE::Texture
	{
		CE_CLASS(Texture2D, CE::Texture)
	public:

		RPI::Texture* GetRpiTexture() override;

	protected:

		FIELD()
		TextureColorSpace colorSpace = TextureColorSpace::None;

		FIELD()
		u32 arrayCount = 1;

#if PAL_TRAIT_BUILD_EDITOR
		friend class CE::Editor::TextureAssetImportJob;
#endif
	};
    
} // namespace CE

#include "Texture2D.rtti.h"
