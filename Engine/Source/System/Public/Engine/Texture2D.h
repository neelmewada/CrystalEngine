#pragma once

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
	namespace Editor { class TextureAssetImporter; }
#endif

	CLASS()
	class SYSTEM_API Texture2D : public Texture
	{
		CE_CLASS(Texture2D, Texture)
	public:


	protected:

		FIELD()
		TextureFormat format = TextureFormat::None;

#if PAL_TRAIT_BUILD_EDITOR
		friend class CE::Editor::TextureAssetImporter;
#endif
	};
    
} // namespace CE

#include "Texture2D.rtti.h"
