#pragma once

namespace CE::RHI
{

	class CORERHI_API Buffer : public Resource, public IDeviceObject
	{
	protected:
		Buffer() : Resource(ResourceType::Buffer), IDeviceObject(DeviceObjectType::Buffer)
		{}

	public:
		virtual ~Buffer() = default;

		virtual BufferBindFlags GetBindFlags() = 0;

		virtual void* GetHandle() = 0;

		virtual u64 GetBufferSize() = 0;

		virtual void UploadData(const BufferData& data) = 0;

		virtual void ReadData(u8** outData, u64* outDataSize) = 0;

		virtual void Resize(u64 newBufferSize) = 0;

	};
    
} // namespace CE::RHI
