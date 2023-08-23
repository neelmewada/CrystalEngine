
#include "CoreMedia.h"

#include "lodepng.h"
#include "lodepng_util.h"

#if PAL_TRAIT_BUILD_EDITOR
#define CMP_STATIC
#include "compressonator.h"
#endif

namespace CE
{
	static const u8 pngHeader[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, };

    CMImage::CMImage()
    {
		
    }

    CMImage::~CMImage()
    {

    }

    CMImageInfo CMImage::GetImageInfoFromFile(const IO::Path& filePath)
    {
        CMImageInfo info{};
        if (!filePath.Exists() || filePath.IsDirectory())
        {
            info.failureReason = "File doesn't exist";
            return info;
        }

        auto filePathStr = filePath.GetString();
		FileStream stream = FileStream(filePath, Stream::Permissions::ReadOnly);
		stream.SetBinaryMode(true);

		if (stream.GetLength() < 8)
		{
			info.failureReason = "Invalid file";
			return info;
		}

		u8 bytes[8] = {};
		stream.Read(bytes, 8);

		bool isPng = true;
		for (int i = 0; i < 8; i++)
		{
			if (bytes[i] != pngHeader[i])
			{
				isPng = false;
				break;
			}
		}

		if (isPng) // Is PNG
		{
			stream.Seek(0);
			MemoryStream memStream = MemoryStream(stream.GetLength());
			stream.Read(memStream.GetRawDataPtr(), stream.GetLength());
			return GetPNGImageInfo(&memStream);
		}

		info.failureReason = "Unsupported image format";
        return info;
    }

    CMImageInfo CMImage::GetImageInfoFromMemory(unsigned char* buffer, int bufferLength)
    {
		CMImageInfo info{};

		if (bufferLength < 8)
		{
			info.failureReason = "Invalid input buffer";
			return info;
		}

		bool isPng = true;
		for (int i = 0; i < 8; i++)
		{
			if (buffer[i] != pngHeader[i])
			{
				isPng = false;
				break;
			}
		}

		if (isPng)
		{
			MemoryStream stream = MemoryStream(buffer, bufferLength, Stream::Permissions::ReadOnly);
			stream.SetBinaryMode(true);
			return GetPNGImageInfo(&stream);
		}

		info.failureReason = "Unsupported image format";
        return info;
    }

	CMImageInfo CMImage::GetPNGImageInfo(MemoryStream* stream)
	{
		CMImageInfo info{};
		if (!stream->CanRead())
		{
			info.failureReason = "Cannot read stream";
			return info;
		}

		stream->SetBinaryMode(true);

		std::vector<unsigned char> image{};
		u32 w, h;
		lodepng::State state{};

		u32 result = lodepng::decode(image, w, h, state, (unsigned char*)stream->GetRawDataPtr(), stream->GetLength());
		
		info.x = w;
		info.y = h;
		info.numChannels = lodepng_get_channels(&state.info_png.color);
		info.bitDepth = state.info_png.color.bitdepth;
		info.bitsPerPixel = lodepng_get_bpp(&state.info_png.color);

		switch (state.info_png.color.colortype)
		{
		case LCT_GREY:
			info.format = CMImageFormat::R;
			break;
		case LCT_GREY_ALPHA:
			info.format = CMImageFormat::RG;
			break;
		case LCT_RGB:
			info.format = CMImageFormat::RGB;
			break;
		case LCT_RGBA:
			info.format = CMImageFormat::RGBA;
			break;
		default:
			info.failureReason = "Invalid format";
			break;
		}

		return info;
	}

	CMImage CMImage::LoadPNGImage(MemoryStream* stream)
	{
		CMImage image{};
		if (!stream->CanRead())
		{
			image.failureReason = "Cannot read stream";
			return image;
		}

		stream->SetBinaryMode(true);

		std::vector<unsigned char> data{};
		u32 w, h;
		lodepng::State state{};

		int length = stream->GetLength();
		if (length <= 0)
			length = stream->GetCapacity();

		u32 result = lodepng::decode(data, w, h, state, (unsigned char*)stream->GetRawDataPtr(), length);
		if (result > 0)
		{
			image.failureReason = "Decoding failed";
			return image;
		}
		
		image.x = w;
		image.y = h;
		image.numChannels = lodepng_get_channels(&state.info_png.color);
		image.bitDepth = state.info_png.color.bitdepth;
		image.bitsPerPixel = lodepng_get_bpp(&state.info_png.color);
		image.sourceFormat = CMImageSourceFormat::PNG;

		switch (state.info_png.color.colortype)
		{
		case LCT_GREY:
			image.format = CMImageFormat::R;
			break;
		case LCT_GREY_ALPHA:
			image.format = CMImageFormat::RG;
			break;
		case LCT_RGB:
			image.format = CMImageFormat::RGB;
			break;
		case LCT_RGBA:
			image.format = CMImageFormat::RGBA;
			break;
		default:
			image.failureReason = "Invalid format";
			return image;
		}

		image.data = (unsigned char*)Memory::Malloc(data.size());
		memcpy(image.data, data.data(), data.size());

		return image;
	}

    CMImage CMImage::LoadFromFile(IO::Path filePath)
    {
        CMImage image{};
		
        if (!filePath.Exists() || filePath.IsDirectory())
        {
            image.failureReason = "File doesn't exist";
            return image;
        }

		auto filePathStr = filePath.GetString();
		FileStream stream = FileStream(filePath, Stream::Permissions::ReadOnly);
		stream.SetBinaryMode(true);
		
		if (stream.GetLength() < 8)
		{
			image.failureReason = "Invalid file";
			return image;
		}

		u8 bytes[8] = {};
		stream.Read(bytes, 8);

		bool isPng = true;
		for (int i = 0; i < 8; i++)
		{
			if (bytes[i] != pngHeader[i])
			{
				isPng = false;
				break;
			}
		}

		if (isPng) // Is PNG
		{
			stream.Seek(0);
			MemoryStream memStream = MemoryStream(stream.GetLength());
			stream.Read(memStream.GetRawDataPtr(), stream.GetLength());
			return LoadPNGImage(&memStream);
		}

		image.failureReason = "Unsupported image format";
        return image;
    }

    CMImage CMImage::LoadFromMemory(unsigned char* buffer, int bufferLength)
    {
		CMImage image{};

		if (bufferLength < 8)
		{
			image.failureReason = "Invalid buffer length";
			return image;
		}

		bool isPng = true;
		for (int i = 0; i < 8; i++)
		{
			if (buffer[i] != pngHeader[i])
			{
				isPng = false;
				break;
			}
		}

		if (isPng) // Is PNG
		{
			MemoryStream memStream = MemoryStream(buffer, bufferLength, Stream::Permissions::ReadOnly);
			return LoadPNGImage(&memStream);
		}

		image.failureReason = "Unsupported image format";
		return image;
    }

	CMImage CMImage::LoadRawImageFromMemory(unsigned char* buffer, CMImageFormat pixelFormat, u32 bitDepth, u32 bitsPerPixel)
	{
		CMImage image{};
		image.allocated = false;
		image.data = buffer;
		image.bitDepth = bitDepth;
		image.bitsPerPixel = bitsPerPixel;
		image.format = pixelFormat;
		image.sourceFormat = CMImageSourceFormat::None;

		return image;
	}

	static CMP_FORMAT CMImageFormatToSourceCMPFormat(CMImageFormat format, u32 bitDepth, u32 bitsPerPixel)
	{
		switch (format)
		{
		case CE::CMImageFormat::R:
			if (bitDepth == 8)
				return CMP_FORMAT_R_8;
			else if (bitDepth == 16)
				return CMP_FORMAT_R_16;
			else if (bitDepth == 32)
				return CMP_FORMAT_R_32F;
			break;
		case CE::CMImageFormat::RG:
			if (bitDepth == 8)
				return CMP_FORMAT_RG_8;
			else if (bitDepth == 16)
				return CMP_FORMAT_RG_16;
			else if (bitDepth == 32)
				return CMP_FORMAT_RG_32F;
			break;
		case CE::CMImageFormat::RGB:
			if (bitDepth == 8)
				return CMP_FORMAT_RGB_888;
			break;
		case CE::CMImageFormat::RGBA:
			if (bitDepth == 8)
				return CMP_FORMAT_RGBA_8888;
			else if (bitDepth == 16)
				return CMP_FORMAT_RGBA_16;
			break;
		default:
			break;
		}

		return CMP_FORMAT_Unknown;
	}

	static CMP_FORMAT CMImageFormatToBCnFormat(CMImageFormat format, u32 bitDepth, u32 bitsPerPixel)
	{
		switch (format)
		{
		case CE::CMImageFormat::R:
			if (bitDepth == 8)
				return CMP_FORMAT_BC4;
			break;
		case CE::CMImageFormat::RG:
			break;
		case CE::CMImageFormat::RGB:
			break;
		case CE::CMImageFormat::RGBA:
			if (bitDepth == 8)
				return CMP_FORMAT_BC7;
			break;
		}

		return CMP_FORMAT_Unknown;
	}

	static CMImageSourceFormat CMPFormatToCMImageSourceFormat(CMP_FORMAT format)
	{
		switch (format)
		{
		case CMP_FORMAT_BC7:
			return CMImageSourceFormat::BC7;
		case CMP_FORMAT_BC4:
			return CMImageSourceFormat::BC4;
		case CMP_FORMAT_BC6H:
			return CMImageSourceFormat::BC6H;
		}
		return CMImageSourceFormat::None;
	}

	static CMP_FORMAT CalculateDestinationCMPFormat(CMImageFormat format, CMImageSourceFormat outputFormat, u32 bitDepth, u32 bitsPerPixel)
	{
		switch (outputFormat)
		{
		case CMImageSourceFormat::BC1:
			return CMP_FORMAT_BC1;
		case CMImageSourceFormat::BC3:
			return CMP_FORMAT_BC3;
		case CMImageSourceFormat::BC4:
			return CMP_FORMAT_BC4;
		case CMImageSourceFormat::BC5:
			return CMP_FORMAT_BC5;
		case CMImageSourceFormat::BC6H:
			return CMP_FORMAT_BC6H;
		case CMImageSourceFormat::BC7:
			return CMP_FORMAT_BC7;
		}

		switch (format)
		{
		case CMImageFormat::R:
			if (bitDepth == 8)
				return CMP_FORMAT_R_8;
			else if (bitDepth == 16)
				return CMP_FORMAT_R_16;
			break;
		case CMImageFormat::RG:
			if (bitDepth == 8)
				return CMP_FORMAT_RG_8;
			else if (bitDepth == 16)
				return CMP_FORMAT_RG_16;
			else if (bitDepth == 32)
				return CMP_FORMAT_RG_32F;
			break;
		case CMImageFormat::RGB:
			if (bitDepth == 8)
				return CMP_FORMAT_RGB_888;
			else if (bitDepth == 32)
				return CMP_FORMAT_RGB_32F;
			break;
		case CMImageFormat::RGBA:
			if (bitDepth == 8)
				return CMP_FORMAT_RGBA_8888;
			else if (bitDepth == 16)
				return CMP_FORMAT_RGBA_16F;
			else if (bitDepth == 32)
				return CMP_FORMAT_RGBA_32F;
			break;
		}
		
		return CMP_FORMAT_Unknown;
	}

	static bool IsFormatSupported(CMImageFormat pixelFormat, u32 bitDepth, CMImageSourceFormat outputFormat)
	{
		switch (pixelFormat)
		{
		case CMImageFormat::R:
			if (outputFormat == CMImageSourceFormat::BC4 && bitDepth == 8)
				return true;
			break;
		case CMImageFormat::RG:
			if (outputFormat == CMImageSourceFormat::BC5 && bitDepth == 8)
				return true;
			break;
		case CMImageFormat::RGB:
			if ((outputFormat == CMImageSourceFormat::BC1 || outputFormat == CMImageSourceFormat::BC7) && bitDepth == 8)
				return true;
			break;
		case CMImageFormat::RGBA:
			if ((outputFormat == CMImageSourceFormat::BC3 || outputFormat == CMImageSourceFormat::BC7) && bitDepth == 8)
				return true;
			break;
		}

		return false;
	}

#if PAL_TRAIT_BUILD_EDITOR

	bool CMImage::EncodeToBCn(const CMImage& source, Stream* outStream, CMImageSourceFormat destFormat, float quality)
	{
		if (!source.IsValid() || outStream == nullptr || !outStream->CanWrite())
			return false;
		if ((int)destFormat < (int)CMImageSourceFormat::BC1 ||
			(int)destFormat > (int)CMImageSourceFormat::BC7)
			return false;
		if (!IsFormatSupported(source.format, source.bitDepth, destFormat))
			return false;

		CMP_Texture src{};
		src.dwWidth = source.GetWidth();
		src.dwHeight = source.GetHeight();
		src.dwSize = sizeof(CMP_Texture);
		src.format = CMImageFormatToSourceCMPFormat(source.format, source.bitDepth, source.bitsPerPixel);
		src.pData = (CMP_BYTE*)source.data;
		src.dwDataSize = source.GetDataSize();

		if (src.format == CMP_FORMAT_Unknown)
			return false;

		CMP_Texture dest{};
		dest.dwPitch = 0;
		dest.dwWidth = source.GetWidth();
		dest.dwHeight = source.GetHeight();
		dest.dwSize = sizeof(CMP_Texture);
		dest.format = CalculateDestinationCMPFormat(source.format, destFormat, source.bitDepth, source.bitsPerPixel);
		if (dest.format == CMP_FORMAT_Unknown)
			return false;

		dest.dwDataSize = CMP_CalculateBufferSize(&dest);
		dest.pData = (CMP_BYTE*)Memory::Malloc(dest.dwDataSize);

		defer(
			Memory::Free(dest.pData);
		);

		CMP_CompressOptions options = CMP_CompressOptions();
		options.dwSize = sizeof(options);
		options.nEncodeWith = CMP_GPU_HW;
		options.fquality = quality;

		CMP_ERROR status = CMP_ConvertTexture(&src, &dest, &options, nullptr);
		if (status != CMP_OK)
		{
#if PLATFORM_WINDOWS
			options.nEncodeWith = CMP_GPU_DXC;
			status = CMP_ConvertTexture(&src, &dest, &options, nullptr);
#endif
			if (status != CMP_OK)
			{
				options.nEncodeWith = CMP_GPU_VLK;
				status = CMP_ConvertTexture(&src, &dest, &options, nullptr);
			}

			if (status != CMP_OK)
			{
				options.nEncodeWith = CMP_HPC;
				status = CMP_ConvertTexture(&src, &dest, &options, nullptr);
			}

			CE_LOG(Error, All, "BCn Encoding failed: {}", (int)status);
			return false;
		}

		outStream->Write(dest.pData, dest.dwDataSize);

		return true;
	}
#endif

	bool CMImage::EncodePNG(const CMImage& source, Stream* outStream, CMImageFormat pixelFormat, u32 bitDepth)
	{
		if (!source.IsValid() || outStream == nullptr || !outStream->CanWrite())
			return false;

		std::vector<u8> encoded{};
		lodepng::State state{};
		state.info_raw.bitdepth = bitDepth;
		switch (pixelFormat)
		{
		case CMImageFormat::R:
			state.info_raw.colortype = LCT_GREY;
			break;
		case CMImageFormat::RG:
			state.info_raw.colortype = LCT_GREY_ALPHA;
			break;
		case CMImageFormat::RGB:
			state.info_raw.colortype = LCT_RGB;
			break;
		case CMImageFormat::RGBA:
			state.info_raw.colortype = LCT_RGBA;
			break;
		default:
			CE_LOG(Error, All, "Failed to encode png. Invalid pixel format: {}", (int)pixelFormat);
			return false;
		}

		u32 result = lodepng::encode(encoded, source.data, source.GetWidth(), source.GetHeight(), state);
		if (result > 0)
		{
			CE_LOG(Error, All, "Failed to encode png. {}.", lodepng_error_text(result));
			return false;
		}
		
		for (auto byte : encoded)
		{
			if (outStream->IsOutOfBounds())
			{
				CE_LOG(Error, All, "Failed to encode png. Stream out of bounds.");
				return false;
			}
			outStream->Write(byte);
		}

		return true;
	}

	CMImage CMImage::DecodePNG(const CMImage& source, MemoryStream* inStream)
	{
		if (!source.IsValid() || inStream == nullptr || !inStream->CanRead())
			return CMImage();

		CMImage image{};
		inStream->SetBinaryMode(true);

		std::vector<unsigned char> data{};
		u32 w, h;
		lodepng::State state{};

		int length = inStream->GetLength();
		if (length <= 0)
			length = inStream->GetCapacity();

		u32 result = lodepng::decode(data, w, h, state, (unsigned char*)inStream->GetRawDataPtr(), length);
		if (result > 0)
		{
			image.failureReason = "Decoding failed";
			return image;
		}

		image.x = w;
		image.y = h;
		image.numChannels = lodepng_get_channels(&state.info_png.color);
		image.bitDepth = state.info_png.color.bitdepth;
		image.bitsPerPixel = lodepng_get_bpp(&state.info_png.color);
		image.sourceFormat = CMImageSourceFormat::PNG;

		switch (state.info_png.color.colortype)
		{
		case LCT_GREY:
			image.format = CMImageFormat::R;
			break;
		case LCT_GREY_ALPHA:
			image.format = CMImageFormat::RG;
			break;
		case LCT_RGB:
			image.format = CMImageFormat::RGB;
			break;
		case LCT_RGBA:
			image.format = CMImageFormat::RGBA;
			break;
		default:
			image.failureReason = "Invalid format";
			return image;
		}

		image.data = (unsigned char*)Memory::Malloc(data.size());
		memcpy(image.data, data.data(), data.size());

		return image;
	}

    void CMImage::Free()
    {
		if (data != nullptr && allocated)
		{
			Memory::Free(data);
		}

        data = nullptr;
        x = y = numChannels = 0;
        failureReason = nullptr;
		bitDepth = 0;
		bitsPerPixel = 0;
		format = CMImageFormat::Undefined;
    }

}
