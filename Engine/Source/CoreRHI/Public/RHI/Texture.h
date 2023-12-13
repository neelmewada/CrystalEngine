#pragma once

namespace CE::RHI
{

	class CORERHI_API Texture : public Resource, public IDeviceObject
	{
	protected:
		Texture() : Resource(ResourceType::Texture), IDeviceObject(DeviceObjectType::Texture)
		{}

	public:
		virtual ~Texture() = default;

		virtual void* GetHandle() = 0;

		virtual u32 GetWidth() = 0;
		virtual u32 GetHeight() = 0;
		virtual u32 GetDepth() = 0;
		virtual u32 GetBytesPerChannel() = 0;
		virtual u32 GetNumberOfChannels() = 0;

		virtual void UploadData(const void* pixels, u64 dataSize) = 0;

		virtual void ReadData(u8** outPixels, u64* outDataSize) = 0;
	};
    
} // namespace CE::RHI
