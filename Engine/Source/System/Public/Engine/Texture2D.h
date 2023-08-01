#pragma once

namespace CE
{
	CLASS()
	class SYSTEM_API Texture2D : public Texture
	{
		CE_CLASS(Texture2D, Texture)
	public:


	protected:

		FIELD()
		TextureFormat format = TextureFormat::None;

	};
    
} // namespace CE

#include "Texture2D.rtti.h"
