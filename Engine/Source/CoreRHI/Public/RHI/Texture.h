#pragma once

namespace CE::RHI
{

	struct TextureDescriptor
	{
		Name name{};
		u32 width = 0, height = 0, depth = 1;
		Dimension dimension = Dimension::Dim2D;
		Format format{};
		u32 mipLevels = 1;
		u32 sampleCount = 1;
		TextureBindFlags bindFlags = TextureBindFlags::ShaderRead;
	};

	class CORERHI_API Texture : public RHIResource, public IDeviceObject
	{
	protected:
		Texture() : RHIResource(ResourceType::Texture), IDeviceObject(DeviceObjectType::Texture)
		{}

	public:
		virtual ~Texture() = default;

		virtual void* GetHandle() = 0;
        
        inline u32 GetWidth() const
        {
            return width;
        }
        
		inline u32 GetHeight() const
        {
            return height;
        }
        
		inline u32 GetDepth() const
        {
            return depth;
        }
        
		inline u32 GetBytesPerChannel() = 0;
        
		virtual u32 GetNumberOfChannels() = 0;

		virtual void UploadData(const void* pixels, u64 dataSize) = 0;

		virtual void ReadData(u8** outPixels, u64* outDataSize) = 0;

	protected:

		Name name{};

		u32 width = 0, height = 0, depth = 0;
		Dimension dimension = Dimension::Dim2D;
		Format format{};
		u32 mipLevels = 1;
		u32 sampleCount = 1;
		TextureBindFlags bindFlags = TextureBindFlags::None;
		
	};
    
} // namespace CE::RHI
