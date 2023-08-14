#pragma once

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
	namespace Editor { class TextureImportJob; }
#endif

	CLASS()
	class SYSTEM_API Texture2D : public Texture
	{
		CE_CLASS(Texture2D, Texture)
	public:


	protected:

		FIELD()
		TextureFormat pixelFormat = TextureFormat::None;



#if PAL_TRAIT_BUILD_EDITOR
		friend class CE::Editor::TextureImportJob;
#endif
	};
    
} // namespace CE

#include "Texture2D.rtti.h"
