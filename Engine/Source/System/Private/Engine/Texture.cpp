#include "System.h"

namespace CE::Private
{
	TextureSource::TextureSource()
	{

	}

	bool TextureSource::IsValid()
	{
		return rawData.IsValid();
	}

	void TextureSource::Release()
	{
		rawData.Free();
	}
}

namespace CE
{

	Texture::Texture()
	{
		
	}

	Texture::~Texture()
	{

	}
    
} // namespace CE


