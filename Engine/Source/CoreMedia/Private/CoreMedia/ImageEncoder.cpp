#include "CoreMedia.h"

#if PLATFORM_DESKTOP
#define CMP_STATIC
#include "compressonator.h"
#endif

namespace CE
{
	static Atomic<bool> cmpLoaded = false;

	CMImageEncoder::CMImageEncoder()
	{

	}

	CMImageEncoder::~CMImageEncoder()
	{

	}

#if PLATFORM_DESKTOP
	static CMP_FORMAT CMImageFormatToSourceCMPFormat(CMImageFormat format, u32 bitDepth, u32 bitsPerPixel)
	{
		switch (format)
		{
		case CE::CMImageFormat::R8:
			if (bitDepth == 8)
				return CMP_FORMAT_R_8;
			else if (bitDepth == 16)
				return CMP_FORMAT_R_16;
			else if (bitDepth == 32)
				return CMP_FORMAT_R_32F;
			break;
		case CE::CMImageFormat::R32:
			return CMP_FORMAT_R_32F;
		case CE::CMImageFormat::RG8:
			if (bitDepth == 8)
				return CMP_FORMAT_RG_8;
			else if (bitDepth == 16)
				return CMP_FORMAT_RG_16;
			else if (bitDepth == 32)
				return CMP_FORMAT_RG_32F;
			break;
		case CE::CMImageFormat::RG32:
			return CMP_FORMAT_RG_32F;
		case CE::CMImageFormat::RGB8:
			if (bitDepth == 8)
				return CMP_FORMAT_RGB_888;
			break;
		case CE::CMImageFormat::RGB32:
			return CMP_FORMAT_RGB_32F;
		case CE::CMImageFormat::RGBA8:
			if (bitDepth == 8)
				return CMP_FORMAT_RGBA_8888;
			else if (bitDepth == 16)
				return CMP_FORMAT_RGBA_16;
			break;
		case CE::CMImageFormat::RGBA32:
			return CMP_FORMAT_RGBA_32F;
		default:
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
		case CMImageFormat::R8:
			if (bitDepth == 8)
				return CMP_FORMAT_R_8;
			else if (bitDepth == 16)
				return CMP_FORMAT_R_16;
			break;
		case CMImageFormat::RG8:
			if (bitDepth == 8)
				return CMP_FORMAT_RG_8;
			else if (bitDepth == 16)
				return CMP_FORMAT_RG_16;
			else if (bitDepth == 32)
				return CMP_FORMAT_RG_32F;
			break;
		case CMImageFormat::RGB8:
			if (bitDepth == 8)
				return CMP_FORMAT_RGB_888;
			else if (bitDepth == 32)
				return CMP_FORMAT_RGB_32F;
			break;
		case CMImageFormat::RGBA8:
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

	static bool IsFormatSupportedForBCn(CMImageFormat pixelFormat, u32 bitDepth, CMImageSourceFormat outputFormat)
	{
		switch (pixelFormat)
		{
		case CMImageFormat::R8:
			if (outputFormat == CMImageSourceFormat::BC4 && bitDepth == 8)
				return true;
			break;
		case CMImageFormat::RG8:
			if (outputFormat == CMImageSourceFormat::BC5 && bitDepth == 8)
				return true;
			break;
		case CMImageFormat::RGB8:
			if ((outputFormat == CMImageSourceFormat::BC1 || outputFormat == CMImageSourceFormat::BC7) && bitDepth == 8)
				return true;
			break;
		case CMImageFormat::RGBA8:
			if ((outputFormat == CMImageSourceFormat::BC3 || outputFormat == CMImageSourceFormat::BC7) && bitDepth == 8)
				return true;
			break;
		case CMImageFormat::RGB32:
		case CMImageFormat::RGBA32:
			if (outputFormat == CMImageSourceFormat::BC6H)
				return true;
			break;
		}

		return false;
	}
#endif


	bool CMImageEncoder::EncodeToBCn(const IO::Path& sourceFile, BinaryBlob& outData, CMImageSourceFormat destFormat, float quality)
	{
#if PLATFORM_DESKTOP
		errorMessage = "";

		if (!cmpLoaded)
		{
			cmpLoaded = true;
			CMP_InitFramework();
		}

		if (!sourceFile.Exists())
			return false;
		if ((int)destFormat < (int)CMImageSourceFormat::BC1 ||
			(int)destFormat >(int)CMImageSourceFormat::BC7)
		{
			errorMessage = "Input format is not BCn";
			return false;
		}

		CMP_ERROR result = CMP_OK;

		String sourceFileStr = sourceFile.GetString();

		CMP_MipSet input;
		memset(&input, 0, sizeof(input));

		result = CMP_LoadTexture(sourceFileStr.GetCString(), &input);
		if (result != CMP_OK)
		{
			errorMessage = "Failed to load texture";
			return false;
		}

		CMP_MipSet output;
		memset(&output, 0, sizeof(output));

		/*input.dwWidth = source.GetWidth();
		input.dwHeight = source.GetHeight();
		input.dwDataSize = source.GetDataSize();
		input.m_TextureType = TT_2D;
		switch (source.GetFormat())
		{
		case CMImageFormat::RGBA32:
			input.m_TextureDataType = TDT_ARGB;
			input.m_format = CMP_FORMAT_RGBA_32F;
			input.m_ChannelFormat = CF_Float32;
			input.m_nChannels = 4;
			break;
		case CMImageFormat::RGBA8:
			input.m_TextureDataType = TDT_ARGB;
			input.m_format = CMP_FORMAT_RGBA_8888;
			input.m_ChannelFormat = CF_8bit;
			input.m_nChannels = 4;
			break;
		case CMImageFormat::RGB32:
			input.m_TextureDataType = TDT_RGB;
			input.m_format = CMP_FORMAT_RGB_32F;
			input.m_ChannelFormat = CF_Float32;
			input.m_nChannels = 3;
			break;
		case CMImageFormat::RGB8:
			input.m_TextureDataType = TDT_RGB;
			input.m_format = CMP_FORMAT_RGB_888;
			input.m_ChannelFormat = CF_8bit;
			input.m_nChannels = 3;
			break;
		case CMImageFormat::RG32:
			input.m_TextureDataType = TDT_RG;
			input.m_format = CMP_FORMAT_RG_32F;
			input.m_ChannelFormat = CF_Float32;
			input.m_nChannels = 2;
			break;
		case CMImageFormat::RG8:
			input.m_TextureDataType = TDT_RG;
			input.m_format = CMP_FORMAT_RG_8;
			input.m_ChannelFormat = CF_8bit;
			input.m_nChannels = 2;
			break;
		case CMImageFormat::R32:
			input.m_TextureDataType = TDT_R;
			input.m_format = CMP_FORMAT_R_32F;
			input.m_ChannelFormat = CF_Float32;
			input.m_nChannels = 1;
			break;
		case CMImageFormat::R8:
			input.m_TextureDataType = TDT_R;
			input.m_format = CMP_FORMAT_R_8;
			input.m_ChannelFormat = CF_8bit;
			input.m_nChannels = 1;
			break;
		}

		if ((input.dwWidth % 4) > 0 || (input.dwHeight % 4) > 0)
		{
			errorMessage = "Input image width or height not divisible by 4";
			return false;
		}

		input.m_nWidth = input.dwWidth;
		input.m_nHeight = input.dwHeight;
		input.m_nDepth = 1;
		input.m_nMipLevels = 1;
		input.m_nMaxMipLevels = 1;

		input.pData = (CMP_BYTE*)source.GetDataPtr();*/

		CMP_FORMAT destCmpFormat = CMP_FORMAT_Unknown;
		switch (destFormat)
		{
		case CMImageSourceFormat::BC1:
			destCmpFormat = CMP_FORMAT_BC1;
			break;
		case CMImageSourceFormat::BC3:
			destCmpFormat = CMP_FORMAT_BC3;
			break;
		case CMImageSourceFormat::BC4:
			destCmpFormat = CMP_FORMAT_BC4;
			break;
		case CMImageSourceFormat::BC5:
			destCmpFormat = CMP_FORMAT_BC5;
			break;
		case CMImageSourceFormat::BC6H:
			destCmpFormat = CMP_FORMAT_BC6H;
			break;
		case CMImageSourceFormat::BC7:
			destCmpFormat = CMP_FORMAT_BC7;
			break;
		default:
			return false;
		}

		CMP_INT requestMipLevels = 1;
		CMP_INT nMinSize = CMP_CalcMinMipSize(input.m_nHeight, input.m_nWidth, requestMipLevels);
		CMP_INT mipResult = CMP_GenerateMIPLevels(&input, nMinSize);

		KernelOptions kernelOptions{};
		memset(&kernelOptions, 0, sizeof(kernelOptions));
		kernelOptions.encodeWith = CMP_GPU_HW;
		kernelOptions.format = destCmpFormat;
		kernelOptions.fquality = quality;

		result = CMP_ProcessTexture(&input, &output, kernelOptions, nullptr);

		if (result != CMP_OK)
		{
			errorMessage = String("CMP error: ") + (int)result;
			return false;
		}

		outData.LoadData(output.pData, output.dwDataSize);

		CMP_FreeMipSet(&input);
		CMP_FreeMipSet(&output);

		return true;
#else // !PLATFORM_DESKTOP
		errorMessage = "Unsupported Platform";
		return false;
#endif
	}

} // namespace CE
