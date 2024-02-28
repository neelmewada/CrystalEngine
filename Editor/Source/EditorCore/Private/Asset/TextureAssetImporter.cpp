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
			job->importHdrAsCubemap = importHdrAsCubemap;

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
		
		String extension = sourcePath.GetFilename().GetExtension().GetString();
		String fileName = sourcePath.GetFilename().RemoveExtension().GetString();
		// Make sure we can use the fileName as name of an object
		fileName = FixObjectName(fileName);

		CMImage image = CMImage::LoadFromFile(sourcePath);
		if (!image.IsValid())
			return false;

		defer(
			image.Free();
		);
		
		bool isCubeMap = false;
		if (extension == ".hdr" && image.GetWidth() == image.GetHeight() * 2)
		{
			//isCubeMap = importHdrAsCubemap;
		}

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
				pixelFormat = TextureFormat::BC4;
				targetSourceFormat = CMImageSourceFormat::BC4;
				compressionFormat = TextureSourceCompressionFormat::BC4;
				break;
			case CMImageFormat::RG8:
				pixelFormat = TextureFormat::BC5;
				targetSourceFormat = CMImageSourceFormat::BC5;
				compressionFormat = TextureSourceCompressionFormat::BC5;
				break;
			case CMImageFormat::RGB8:
				if (compressionQuality == TextureCompressionQuality::High)
				{
					pixelFormat = TextureFormat::BC7;
					targetSourceFormat = CMImageSourceFormat::BC7;
					compressionFormat = TextureSourceCompressionFormat::BC7;
				}
				else
				{
					pixelFormat = TextureFormat::BC1;
					targetSourceFormat = CMImageSourceFormat::BC1;
					compressionFormat = TextureSourceCompressionFormat::BC1;
				}
				break;
			case CMImageFormat::RGBA8:
				if (compressionQuality == TextureCompressionQuality::High)
				{
					pixelFormat = TextureFormat::BC7;
					targetSourceFormat = CMImageSourceFormat::BC7;
					compressionFormat = TextureSourceCompressionFormat::BC7;
				}
				else
				{
					pixelFormat = TextureFormat::BC3;
					targetSourceFormat = CMImageSourceFormat::BC3;
					compressionFormat = TextureSourceCompressionFormat::BC3;
				}
				break;
			case CMImageFormat::RGB32:
			case CMImageFormat::RGBA32:
				pixelFormat = TextureFormat::BC6H;
				targetSourceFormat = CMImageSourceFormat::BC6H;
				targetSourceFormat = CMImageSourceFormat::BC6H;
				compressionFormat = TextureSourceCompressionFormat::BC6H;
				break;
			}
		}

		if (isCubeMap) // Process CubeMap
		{
			return ProcessCubeMap(fileName, package, image, pixelFormat, compressionFormat, targetSourceFormat);
		}
		else
		{
			return ProcessTex2D(fileName, package, image, pixelFormat, compressionFormat, targetSourceFormat);
		}

		return true;
	}

	bool TextureAssetImportJob::ProcessCubeMap(const String& name, Package* package, const CMImage& sourceImage, TextureFormat pixelFormat,
		TextureSourceCompressionFormat compressionFormat,
		CMImageSourceFormat targetSourceFormat)
	{
		TextureCube* texture = CreateObject<TextureCube>(package, name);

		texture->anisoLevel = anisotropy;
		texture->width = texture->height = sourceImage.GetHeight();
		texture->mipLevels = 1;
		texture->addressModeU = texture->addressModeV = TextureAddressMode::Repeat;
		texture->filter = RHI::FilterMode::Linear;
		texture->pixelFormat = pixelFormat;
		texture->compressionQuality = compressionQuality;
		texture->sourceCompressionFormat = compressionFormat;

		RPI::CubeMapProcessInfo processInfo{};
		processInfo.name = name;
		processInfo.preferredFormat = RHI::Format::R16G16B16A16_SFLOAT;
		processInfo.sourceImage = sourceImage;
		processInfo.diffuseIrradianceResolution = 0;
		processInfo.equirectangularShader = gEngine->GetEquirectProjectionShader()->GetOrCreateRPIShader(0);

		RPI::CubeMap* cubeMap = RPI::CubeMap::ProcessCubeMap(processInfo);
		if (cubeMap == nullptr)
		{
			errorMessage = "Failed to process CubeMap";
			return false;
		}

		return true;
	}

	bool TextureAssetImportJob::ProcessTex2D(const String& name, Package* package, const CMImage& image, TextureFormat pixelFormat,
		TextureSourceCompressionFormat compressionFormat,
		CMImageSourceFormat targetSourceFormat)
	{
		Texture2D* texture = CreateObject<Texture2D>(package, name);

		texture->anisoLevel = anisotropy;
		texture->width = image.GetWidth();
		texture->height = image.GetHeight();
		texture->arrayCount = 1;
		texture->mipLevels = 1;
		texture->addressModeU = texture->addressModeV = TextureAddressMode::Repeat;
		texture->filter = RHI::FilterMode::Linear;
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
			u32 channelStride = numChannels;
			if (numChannels == 3)
				channelStride = 4;

			bool isFloat = false;
			switch (image.GetFormat())
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
					f32 r = *((f32*)image.GetDataPtr() + channelStride * i + 0);
					f32 g = 0;
					if (channelStride >= 2)
						g = *((f32*)image.GetDataPtr() + channelStride * i + 1);
					f32 b = 0;
					if (channelStride >= 3)
						b = *((f32*)image.GetDataPtr() + channelStride * i + 2);
					f32 a = 0;
					if (channelStride >= 4)
						a = *((f32*)image.GetDataPtr() + channelStride * i + 3);

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
					u8 r = *((u8*)image.GetDataPtr() + channelStride * i + 0);
					u8 g = 0;
					if (channelStride >= 2)
						g = *((u8*)image.GetDataPtr() + channelStride * i + 1);
					u8 b = 0;
					if (channelStride >= 3)
						b = *((u8*)image.GetDataPtr() + channelStride * i + 2);
					u8 a = 0;
					if (channelStride >= 4)
						a = *((u8*)image.GetDataPtr() + channelStride * i + 3);

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
