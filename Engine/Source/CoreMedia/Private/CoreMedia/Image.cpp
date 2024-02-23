
#include "CoreMedia.h"

#include "lodepng.h"
#include "lodepng_util.h"

#if PLATFORM_DESKTOP
#define CMP_STATIC
#include "compressonator.h"
#endif

#include "stb_image.h"

namespace CE
{
	static const u8 pngHeader[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, };
	static const u8 jpgHeader[3] = { 0xFF, 0xD8, 0xFF };
	static const u8 hdrHeader[10] = { '#', '?', 'R', 'A', 'D', 'I', 'A', 'N', 'C', 'E' }; // #?RADIANCE
	static const u8 exrHeader[4] = { 0x76, 0x2F, 0x31, 0x01 };

	static CMImageSourceFormat GetSourceFormatFromHeader(u8 header[10])
	{
		bool isPNG = true;
		for (int i = 0; i < COUNTOF(pngHeader); i++)
		{
			if (header[i] != pngHeader[i])
			{
				isPNG = false;
				break;
			}
		}

		if (isPNG)
			return CMImageSourceFormat::PNG;

		bool isJPG = true;
		for (int i = 0; i < COUNTOF(jpgHeader); i++)
		{
			if (header[i] != jpgHeader[i])
			{
				isJPG = false;
				break;
			}
		}

		if (isJPG)
			return CMImageSourceFormat::JPG;

		bool isHDR = true;
		for (int i = 0; i < COUNTOF(hdrHeader); i++)
		{
			if (header[i] != hdrHeader[i])
			{
				isHDR = false;
				break;
			}
		}

		if (isHDR)
			return CMImageSourceFormat::HDR;

		bool isEXR = true;
		for (int i = 0; i < COUNTOF(exrHeader); i++)
		{
			if (header[i] != exrHeader[i])
			{
				isEXR = false;
				break;
			}
		}

		if (isEXR)
			return CMImageSourceFormat::EXR;

		return CMImageSourceFormat::None;
	}
	
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

		u8 bytes[10] = {};
		stream.Read(bytes, 10);
		stream.Seek(0);

		auto sourceType = GetSourceFormatFromHeader(bytes);

		if (sourceType == CMImageSourceFormat::PNG) // Is PNG
		{
			MemoryStream memStream = MemoryStream(stream.GetLength());
			stream.Read(memStream.GetRawDataPtr(), stream.GetLength());
			return GetPNGImageInfo(&memStream);
		}
		else if (sourceType == CMImageSourceFormat::JPG)
		{
			MemoryStream memStream = MemoryStream(stream.GetLength());
			stream.Read(memStream.GetRawDataPtr(), stream.GetLength());
			return GetJPGImageInfo(&memStream);
		}

		info.failureReason = "Unsupported image format";
        return info;
    }

    CMImageInfo CMImage::GetImageInfoFromMemory(unsigned char* buffer, u32 bufferLength)
    {
		CMImageInfo info{};

		if (bufferLength < 10)
		{
			info.failureReason = "Invalid input buffer";
			return info;
		}

		auto sourceType = GetSourceFormatFromHeader(buffer);

		if (sourceType == CMImageSourceFormat::PNG)
		{
			MemoryStream stream = MemoryStream(buffer, bufferLength, Stream::Permissions::ReadOnly);
			stream.SetBinaryMode(true);
			return GetPNGImageInfo(&stream);
		}
		else if (sourceType == CMImageSourceFormat::JPG)
		{
			MemoryStream stream = MemoryStream(buffer, bufferLength, Stream::Permissions::ReadOnly);
			stream.SetBinaryMode(true);
			return GetJPGImageInfo(&stream);
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
			info.format = CMImageFormat::R8;
			break;
		case LCT_GREY_ALPHA:
			info.format = CMImageFormat::RG8;
			break;
		case LCT_RGB:
			info.format = CMImageFormat::RGB8;
			break;
		case LCT_RGBA:
			info.format = CMImageFormat::RGBA8;
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

		int x = 0, y = 0, channels = 0;
		stbi_info_from_memory((const stbi_uc*)stream->GetRawDataPtr(), stream->GetLength(), &x, &y, &channels);

		unsigned char* imageData = stbi_load_from_memory((const stbi_uc*)stream->GetRawDataPtr(), stream->GetLength(), &x, &y, &channels, channels);

		if (channels == 1)
			image.format = CMImageFormat::R8;
		else if (channels == 2)
			image.format = CMImageFormat::RG8;
		else if (channels == 3)
			image.format = CMImageFormat::RGB8;
		else if (channels == 4)
			image.format = CMImageFormat::RGBA8;

		image.x = x;
		image.y = y;
		image.numChannels = channels;
		image.bitsPerPixel = 8 * channels;
		image.bitDepth = 8;
		image.sourceFormat = CMImageSourceFormat::PNG;
		image.data = imageData;

		return image;
	}

	CMImageInfo CMImage::GetJPGImageInfo(MemoryStream* stream)
	{
		CMImageInfo info{};
		if (!stream->CanRead())
		{
			info.failureReason = "Cannot read stream";
			return info;
		}

		stream->SetBinaryMode(true);

		int x = 0, y = 0, channels = 0;

		int result = stbi_info_from_memory((const stbi_uc*)stream->GetRawDataPtr(), stream->GetLength(), &x, &y, &channels);

		if (channels == 1)
			info.format = CMImageFormat::R8;
		else if (channels == 2)
			info.format = CMImageFormat::RG8;
		else if (channels == 3)
			info.format = CMImageFormat::RGB8;
		else if (channels == 4)
			info.format = CMImageFormat::RGBA8;

		info.x = x;
		info.y = y;
		info.numChannels = channels;
		info.bitsPerPixel = 8 * channels;
		info.bitDepth = 8;
		info.sourceFormat = CMImageSourceFormat::JPG;

		return info;
	}

	CMImage CMImage::LoadJPGImage(MemoryStream* stream)
	{
		CMImage image{};
		if (!stream->CanRead())
		{
			image.failureReason = "Cannot read stream";
			return image;
		}

		stream->SetBinaryMode(true);

		int x = 0, y = 0, channels = 0;

		unsigned char* imageData = stbi_load_from_memory((const stbi_uc*)stream->GetRawDataPtr(), stream->GetLength(), &x, &y, &channels, 3);

		if (channels == 1)
			image.format = CMImageFormat::R8;
		else if (channels == 2)
			image.format = CMImageFormat::RG8;
		else if (channels == 3)
			image.format = CMImageFormat::RGB8;
		else if (channels == 4)
			image.format = CMImageFormat::RGBA8;

		image.x = x;
		image.y = y;
		image.numChannels = channels;
		image.bitsPerPixel = 8 * channels;
		image.bitDepth = 8;
		image.sourceFormat = CMImageSourceFormat::JPG;
		image.data = imageData;

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
		
		if (stream.GetLength() < 10)
		{
			image.failureReason = "Invalid file";
			return image;
		}

		u8 bytes[10] = {};
		stream.Read(bytes, 10);
		stream.Seek(0);

		auto sourceType = GetSourceFormatFromHeader(bytes);

		if (sourceType == CMImageSourceFormat::PNG) // Is PNG
		{
			stream.Close(); // Close the stream

			int x = 0, y = 0, channels = 0;
			stbi_info(filePathStr.GetCString(), &x, &y, &channels);

			unsigned char* imageData = stbi_load(filePathStr.GetCString(), &x, &y, &channels, 4);

			if (channels == 1)
				image.format = CMImageFormat::R8;
			else if (channels == 2)
				image.format = CMImageFormat::RG8;
			else if (channels == 3)
				image.format = CMImageFormat::RGB8;
			else if (channels == 4)
				image.format = CMImageFormat::RGBA8;

			image.x = x;
			image.y = y;
			image.numChannels = channels;
			image.bitsPerPixel = 8 * channels;
			image.bitDepth = 8;
			image.sourceFormat = CMImageSourceFormat::PNG;
			image.data = imageData;

			return image;
		}
		else if (sourceType == CMImageSourceFormat::JPG) // Is JPG
		{
			stream.Close(); // Close the stream

			int x = 0, y = 0, channels = 0;
			stbi_info(filePathStr.GetCString(), &x, &y, &channels);

			unsigned char* imageData = stbi_load(filePathStr.GetCString(), &x, &y, &channels, STBI_rgb_alpha);

			if (channels == 1)
				image.format = CMImageFormat::R8;
			else if (channels == 2)
				image.format = CMImageFormat::RG8;
			else if (channels == 3)
				image.format = CMImageFormat::RGB8;
			else if (channels == 4)
				image.format = CMImageFormat::RGBA8;

			image.x = x;
			image.y = y;
			image.numChannels = channels;
			image.bitsPerPixel = 8 * channels;
			image.bitDepth = 8;
			image.sourceFormat = CMImageSourceFormat::JPG;
			image.data = imageData;

			return image;
		}
		else if (sourceType == CMImageSourceFormat::HDR)
		{
			stream.Close();

			int x = 0, y = 0, channels = 0;
			stbi_info(filePathStr.GetCString(), &x, &y, &channels);

			float* imageData = stbi_loadf(filePathStr.GetCString(), &x, &y, &channels, STBI_rgb_alpha);

			if (channels == 1)
				image.format = CMImageFormat::R32;
			else if (channels == 2)
				image.format = CMImageFormat::RG32;
			else if (channels == 3)
				image.format = CMImageFormat::RGB32;
			else if (channels == 4)
				image.format = CMImageFormat::RGBA32;
			
			image.x = x;
			image.y = y;
			image.numChannels = channels;
			image.bitsPerPixel = 16 * channels;
			image.bitDepth = 16;
			image.sourceFormat = CMImageSourceFormat::HDR;
			image.data = imageData;

			return image;
		}

		image.failureReason = "Unsupported image format";
        return image;
    }

    CMImage CMImage::LoadFromMemory(unsigned char* buffer, u32 bufferLength)
    {
		CMImage image{};

		if (bufferLength < 10)
		{
			image.failureReason = "Invalid buffer length";
			return image;
		}

		auto sourceType = GetSourceFormatFromHeader(buffer);

		if (sourceType == CMImageSourceFormat::PNG) // Is PNG
		{
			MemoryStream memStream = MemoryStream(buffer, bufferLength, Stream::Permissions::ReadOnly);
			return LoadPNGImage(&memStream);
		}
		else if (sourceType == CMImageSourceFormat::JPG)
		{
			MemoryStream memStream = MemoryStream(buffer, bufferLength, Stream::Permissions::ReadOnly);
			return LoadJPGImage(&memStream);
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

	bool CMImage::EncodeToBCn(const CMImage& source, Stream* outStream, CMImageSourceFormat destFormat, float quality)
	{
#if false
		if (!source.IsValid() || outStream == nullptr || !outStream->CanWrite())
			return false;
		if ((int)destFormat < (int)CMImageSourceFormat::BC1 ||
			(int)destFormat > (int)CMImageSourceFormat::BC7)
			return false;
		if (!IsFormatSupportedForBCn(source.format, source.bitDepth, destFormat))
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
		options.fquality = 0.01f;

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
#else // !PLATFORM_DESKTOP
		return false;
#endif
	}

	bool CMImage::EncodePNG(const CMImage& source, Stream* outStream, CMImageFormat pixelFormat, u32 bitDepth)
	{
		if (!source.IsValid() || outStream == nullptr || !outStream->CanWrite())
			return false;

		std::vector<u8> encoded{};
		lodepng::State state{};
		state.info_raw.bitdepth = bitDepth;
		switch (pixelFormat)
		{
		case CMImageFormat::R8:
			state.info_raw.colortype = LCT_GREY;
			break;
		case CMImageFormat::RG8:
			state.info_raw.colortype = LCT_GREY_ALPHA;
			break;
		case CMImageFormat::RGB8:
			state.info_raw.colortype = LCT_RGB;
			break;
		case CMImageFormat::RGBA8:
			state.info_raw.colortype = LCT_RGBA;
			break;
		default:
			CE_LOG(Error, All, "Failed to encode png. Invalid pixel format: {}", (int)pixelFormat);
			return false;
		}

		u32 result = lodepng::encode(encoded, (unsigned char*)source.data, source.GetWidth(), source.GetHeight(), state);
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
			image.format = CMImageFormat::R8;
			break;
		case LCT_GREY_ALPHA:
			image.format = CMImageFormat::RG8;
			break;
		case LCT_RGB:
			image.format = CMImageFormat::RGB8;
			break;
		case LCT_RGBA:
			image.format = CMImageFormat::RGBA8;
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
			stbi_image_free(data);
		}

        data = nullptr;
        x = y = numChannels = 0;
        failureReason = nullptr;
		bitDepth = 0;
		bitsPerPixel = 0;
		format = CMImageFormat::Undefined;
    }

}
