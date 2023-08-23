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
	SYSTEM_API bool TextureSourceCompressionFormatIsBCn(TextureSourceCompressionFormat sourceCompressionFormat)
	{
		switch (sourceCompressionFormat)
		{
		case TextureSourceCompressionFormat::BC1:
		case TextureSourceCompressionFormat::BC3:
		case TextureSourceCompressionFormat::BC4:
		case TextureSourceCompressionFormat::BC5:
		case TextureSourceCompressionFormat::BC6H:
		case TextureSourceCompressionFormat::BC7:
			return true;
		}

		return false;
	}

	Texture::Texture()
	{
		
	}

	Texture::~Texture()
	{

	}
    
} // namespace CE


