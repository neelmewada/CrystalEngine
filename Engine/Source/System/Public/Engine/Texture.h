#pragma once

namespace CE
{
	namespace Internal
	{
		STRUCT()
		struct SYSTEM_API TextureSource
		{
			CE_STRUCT(TextureSource)
		public:

			TextureSource();

		};
	}

	CLASS()
	class SYSTEM_API Texture : public Asset
	{
		CE_CLASS(Texture, Asset)
	public:

		Texture();
		virtual ~Texture();

	};
    
} // namespace CE

#include "Texture.rtti.h"
