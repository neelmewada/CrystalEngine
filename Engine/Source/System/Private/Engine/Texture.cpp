#include "System.h"

namespace CE
{
	SYSTEM_API RHI::Format CE::ToRHIFormat(CE::TextureFormat format, bool isSrgb)
	{
		switch (format)
		{
		case TextureFormat::RGBA8:
			return isSrgb ? RHI::Format::R8G8B8A8_SRGB : RHI::Format::R8G8B8A8_UNORM;
		case TextureFormat::RGB8:
			return isSrgb ? RHI::Format::R8G8B8_SRGB : RHI::Format::R8G8B8_UNORM;
		case TextureFormat::BGRA8:
			return isSrgb ? RHI::Format::B8G8R8A8_SRGB : RHI::Format::B8G8R8A8_UNORM;
		case TextureFormat::BGR8:
			return isSrgb ? RHI::Format::B8G8R8_SRGB : RHI::Format::B8G8R8_UNORM;
		case TextureFormat::ARGB4444:
			return RHI::Format::A4R4G4B4_UNORM;
		case TextureFormat::R8:
			return isSrgb ? RHI::Format::R8_SRGB : RHI::Format::R8_UNORM;
		case TextureFormat::R16:
			return RHI::Format::R16_UNORM;
		case TextureFormat::RG8:
			return isSrgb ? RHI::Format::R8G8_SRGB : RHI::Format::R8G8_UNORM;
		case TextureFormat::RG16:
			return RHI::Format::R16G16_UNORM;
		case TextureFormat::RHalf:
			return RHI::Format::R16_SFLOAT;
		case TextureFormat::RGHalf:
			return RHI::Format::R16G16_SFLOAT;
		case TextureFormat::RGBAHalf:
			return RHI::Format::R16G16B16A16_SFLOAT;
		case TextureFormat::RFloat:
			return RHI::Format::R32_SFLOAT;
		case TextureFormat::RGFloat:
			return RHI::Format::R32G32_SFLOAT;
		case TextureFormat::RGBAFloat:
			return RHI::Format::R32G32B32A32_SFLOAT;
		case TextureFormat::RGB565:
			return RHI::Format::R5G6B5_UNORM;
		}
		return RHI::Format::Undefined;
	}

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

	CE::Texture::Texture()
	{
		
	}

	CE::Texture::~Texture()
	{
		delete rpiTexture;
	}

	SYSTEM_API RHI::SamplerAddressMode TextureAddressModeToSamplerMode(TextureAddressMode mode)
	{
		switch (mode)
		{
		case TextureAddressMode::Repeat:
			return RHI::SamplerAddressMode::Repeat;
		case TextureAddressMode::Clamp:
			return RHI::SamplerAddressMode::ClampToEdge;
		case TextureAddressMode::ClampToBorder:
			return RHI::SamplerAddressMode::ClampToBorder;
		}

		return RHI::SamplerAddressMode::Repeat;
	}

} // namespace CE


