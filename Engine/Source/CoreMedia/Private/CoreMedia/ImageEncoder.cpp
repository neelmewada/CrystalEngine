#include "CoreMedia.h"

#if PLATFORM_DESKTOP
#include "ispc_texcomp.h"
#endif

namespace CE
{
	// Class used to store functions to specific quality profiles.
	class CompressionProfile
	{
	public:
		std::function<void(bc6h_enc_settings*)> GetBC6() const
		{
			return bc6;
		}

		std::function<void(bc7_enc_settings*)> GetBC7(bool discardAlpha) const
		{
			if (discardAlpha)
			{
				return bc7;
			}

			return bc7Alpha;
		}

		std::function<void(astc_enc_settings*, int block_width, int block_height)> GetASTC(bool discardAlpha) const
		{
			if (discardAlpha)
			{
				return astc;
			}

			return astcAlpha;
		}

		std::function<void(bc6h_enc_settings*)>   bc6;
		std::function<void(bc7_enc_settings*)>    bc7;
		std::function<void(bc7_enc_settings*)>    bc7Alpha;
		std::function<void(astc_enc_settings*, int block_width, int block_height)>   astc;
		std::function<void(astc_enc_settings*, int block_width, int block_height)>   astcAlpha;
	};

	CMImageEncoder::CMImageEncoder()
	{

	}

	CMImageEncoder::~CMImageEncoder()
	{

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

	bool CMImageEncoder::EncodeToBCn(const CMImage& image, BinaryBlob& outData, CMImageSourceFormat destFormat, Quality quality)
	{
#if PLATFORM_DESKTOP
		errorMessage = "";

		if (!image.IsValid())
			return false;

		if ((int)destFormat < (int)CMImageSourceFormat::BC1 ||
			(int)destFormat > (int)CMImageSourceFormat::BC7)
			return false;

		using QualityPair = Pair<Quality, CompressionProfile>;

		static const QualityPair qualityMap[4] = {
			QualityPair{ Quality_Preview, CompressionProfile{
				GetProfile_bc6h_veryfast, GetProfile_ultrafast, GetProfile_alpha_ultrafast, GetProfile_astc_fast, GetProfile_astc_alpha_fast } },
			QualityPair{ Quality_Fast, CompressionProfile{
				GetProfile_bc6h_fast, GetProfile_fast, GetProfile_alpha_fast, GetProfile_astc_fast, GetProfile_astc_alpha_fast } },
			QualityPair{ Quality_Normal, CompressionProfile{
				GetProfile_bc6h_basic, GetProfile_basic, GetProfile_alpha_basic, GetProfile_astc_alpha_slow, GetProfile_astc_alpha_slow } },
			QualityPair{ Quality_Slow, CompressionProfile{
				GetProfile_bc6h_basic, GetProfile_basic, GetProfile_alpha_basic, GetProfile_astc_alpha_slow, GetProfile_astc_alpha_slow } }
		};
		static const CompressionProfile defaultQuality = { GetProfile_bc6h_basic, GetProfile_basic, GetProfile_alpha_basic, GetProfile_astc_alpha_slow, GetProfile_astc_alpha_slow };

		if (!IsFormatSupportedForBCn(image.GetFormat(), image.GetBitDepth(), destFormat))
		{
			errorMessage = "DestFormat not supported";
			return false;
		}
		if (image.GetWidth() % 4 > 0 || image.GetHeight() % 4 > 0)
		{
			errorMessage = "Width or height not divisible by 4";
			return false;
		}
		
		rgba_surface surface{};
		surface.ptr = (u8*)image.GetDataPtr();
		surface.width = image.GetWidth();
		surface.height = image.GetHeight();
		surface.stride = surface.width * 4; // CMImage always loads 4 bytes per pixel no matter how many channels exist

		u32 destSize = 0;
		bc6h_enc_settings bc6Settings{};
		bc7_enc_settings bc7Settings{};
		bool useAlpha = false;

		if (image.format == CMImageFormat::RGBA8 ||
			image.format == CMImageFormat::RGBA16 ||
			image.format == CMImageFormat::RGBA32)
			useAlpha = true;

		switch (destFormat)
		{
		case CMImageSourceFormat::BC1:
			destSize = surface.width * surface.height / 2;
			outData.Reserve(destSize);
			CompressBlocksBC1(&surface, outData.GetDataPtr());
			break;
		case CMImageSourceFormat::BC3:
			destSize = surface.width * surface.height;
			outData.Reserve(destSize);
			CompressBlocksBC3(&surface, outData.GetDataPtr());
			break;
		case CMImageSourceFormat::BC4:
			destSize = surface.width * surface.height / 2;
			outData.Reserve(destSize);
			CompressBlocksBC4(&surface, outData.GetDataPtr());
			break;
		case CMImageSourceFormat::BC5:
			destSize = surface.width * surface.height;
			outData.Reserve(destSize);
			CompressBlocksBC5(&surface, outData.GetDataPtr());
			break;
		case CMImageSourceFormat::BC6H:
			destSize = surface.width * surface.height;
			outData.Reserve(destSize);
			qualityMap[quality].second.GetBC6()(&bc6Settings);
			CompressBlocksBC6H(&surface, outData.GetDataPtr(), &bc6Settings);
			break;
		case CMImageSourceFormat::BC7:
			destSize = surface.width * surface.height;
			outData.Reserve(destSize);
			qualityMap[quality].second.GetBC7(!useAlpha)(&bc7Settings);
			CompressBlocksBC7(&surface, outData.GetDataPtr(), &bc7Settings);
			break;
		default:
			return false; // Should never happen
		}

		return true;
#else // !PLATFORM_DESKTOP
		errorMessage = "Unsupported Platform";
		return false;
#endif
	}

} // namespace CE
