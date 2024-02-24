#include "EditorCore.h"

namespace CE::Editor
{

	Array<AssetImportJob*> TextureAssetImporter::CreateImportJobs(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets)
	{
		Array<AssetImportJob*> jobs{};

		for (int i = 0; i < sourceAssets.GetSize(); i++)
		{
			auto job = new TextureAssetImportJob(this, sourceAssets[i], productAssets[i]);
			job->compressionQuality = compressionQuality;
			job->anisotropy = anisotropy;

			jobs.Add(job);
		}

		return jobs;
	}

	bool TextureAssetImportJob::ProcessAsset(Package* package)
	{
		if (package == nullptr)
			return false;
		if (!sourcePath.Exists())
			return false;

		// Clear the package of any subobjects/assets, we will build the asset from scratch
		package->DestroyAllSubobjects();

		CMImage image = CMImage::LoadFromFile(sourcePath);
		if (!image.IsValid())
			return false;

		defer(
			image.Free();
		);

		CMImageFormat imageFormat = image.GetFormat();
		CMImageSourceFormat targetSourceFormat = CMImageSourceFormat::None;

		TextureFormat pixelFormat = TextureFormat::None;
		TextureSourceCompressionFormat compressionFormat = TextureSourceCompressionFormat::None;

		switch (imageFormat)
		{
		case CMImageFormat::R8:
			pixelFormat = TextureFormat::R8;
			break;
		case CMImageFormat::RG8:
			pixelFormat = TextureFormat::RG8;
			break;
		case CMImageFormat::RGB8:
			pixelFormat = TextureFormat::RGB8;
			break;
		case CMImageFormat::RGBA8:
			pixelFormat = TextureFormat::RGBA8;
			break;
		case CMImageFormat::RG32:
			pixelFormat = TextureFormat::RGFloat;
			break;
		case CMImageFormat::RG16:
			pixelFormat = TextureFormat::RGHalf;
			break;
		case CMImageFormat::RGB16:
		case CMImageFormat::RGBA16:
			pixelFormat = TextureFormat::RGBAHalf;
			break;
		case CMImageFormat::RGB32:
		case CMImageFormat::RGBA32:
			pixelFormat = TextureFormat::RGBAFloat;
			break;
		}
		
		if (compressionQuality != TextureCompressionQuality::None && PlatformMisc::IsDesktopPlatform(targetPlatform))
		{
			switch (imageFormat)
			{
			case CMImageFormat::R8:
				targetSourceFormat = CMImageSourceFormat::BC4;
				compressionFormat = TextureSourceCompressionFormat::BC4;
				break;
			case CMImageFormat::RG8:
				targetSourceFormat = CMImageSourceFormat::BC5;
				compressionFormat = TextureSourceCompressionFormat::BC5;
				break;
			case CMImageFormat::RGB8:
				if (compressionQuality == TextureCompressionQuality::High)
				{
					targetSourceFormat = CMImageSourceFormat::BC7;
					compressionFormat = TextureSourceCompressionFormat::BC7;
				}
				else
				{
					targetSourceFormat = CMImageSourceFormat::BC1;
					compressionFormat = TextureSourceCompressionFormat::BC1;
				}
				break;
			case CMImageFormat::RGBA8:
				targetSourceFormat = CMImageSourceFormat::BC7;
				compressionFormat = TextureSourceCompressionFormat::BC7;
				break;
			case CMImageFormat::RGB32:
			case CMImageFormat::RGBA32:
				targetSourceFormat = CMImageSourceFormat::BC6H;
				targetSourceFormat = CMImageSourceFormat::BC6H;
				compressionFormat = TextureSourceCompressionFormat::BC6H;
				break;
			}
		}
		
		Texture2D* texture = CreateObject<Texture2D>(package, TEXT("Texture2D"));

		texture->anisoLevel = anisotropy;
		texture->width = image.GetWidth();
		texture->height = image.GetHeight();
		texture->arrayCount = 1;
		texture->mipLevels = 1;
		texture->addressModeU = texture->addressModeV = TextureAddressMode::Wrap;
		texture->filter = RHI::FilterMode::Linear;
		texture->dimension = TextureDimension::Tex2D;
		texture->pixelFormat = pixelFormat;
		texture->compressionQuality = compressionQuality;
		texture->sourceCompressionFormat = compressionFormat;

		if (targetSourceFormat != CMImageSourceFormat::None) // Use BCn format
		{
			float quality = 0.05f;
			CMImageEncoder encoder{};
			bool success = encoder.EncodeToBCn(image, texture->source, targetSourceFormat, CMImageEncoder::Quality_Normal);
			if (!success)
			{
				errorMessage = "Failed to encode to BCn format!";
				return false;
			}
		}
		else // Store raw pixels
		{
			MemoryStream stream = MemoryStream(image.GetDataSize(), true);
			stream.SetBinaryMode(true);
			stream.SetAutoResizeIncrement(4_MB);

			u32 width = image.GetWidth();
			u32 height = image.GetHeight();

			u32 numPixels = width * height;
			u32 numChannels = image.GetNumChannels();

			bool isFloat = false;
			switch (imageFormat)
			{
			case CMImageFormat::R32:
			case CMImageFormat::RG32:
			case CMImageFormat::RGB32:
			case CMImageFormat::RGBA32:
				isFloat = true;
				break;
			}

			for (int i = 0; i < numPixels; i++)
			{
				if (isFloat)
				{
					f32 r = *((f32*)image.GetDataPtr() + 4 * i + 0);
					f32 g = *((f32*)image.GetDataPtr() + 4 * i + 1);
					f32 b = *((f32*)image.GetDataPtr() + 4 * i + 2);
					f32 a = *((f32*)image.GetDataPtr() + 4 * i + 3);

					if (compressionQuality == TextureCompressionQuality::None) // Store full float32
					{
						stream << r;
						if (numChannels >= 2)
							stream << g;
						if (numChannels >= 3)
							stream << b;
						if (numChannels >= 4)
							stream << a;
					}
					else // Store as float16 (half)
					{
						stream << Math::ToFloat16(r);
						if (numChannels >= 2)
							stream << Math::ToFloat16(g);
						if (numChannels >= 3)
							stream << Math::ToFloat16(b);
						if (numChannels >= 4)
							stream << Math::ToFloat16(a);
					}
				}
				else
				{
					u8 r = *((u8*)image.GetDataPtr() + 4 * i + 0);
					u8 g = *((u8*)image.GetDataPtr() + 4 * i + 1);
					u8 b = *((u8*)image.GetDataPtr() + 4 * i + 2);
					u8 a = *((u8*)image.GetDataPtr() + 4 * i + 3);

					stream << r;
					if (numChannels >= 2)
						stream << g;
					if (numChannels >= 3)
						stream << b;
					if (numChannels >= 4)
						stream << a;
				}
			}

			texture->source.LoadData(stream.GetRawDataPtr(), stream.GetLength());
		}

		return true;
	}

} // namespace CE::Editor
