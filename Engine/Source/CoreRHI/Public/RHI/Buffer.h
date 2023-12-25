#pragma once

namespace CE::RHI
{

	class CORERHI_API Buffer : public RHIResource, public IDeviceObject
	{
	protected:
		Buffer() : RHIResource(ResourceType::Buffer), IDeviceObject(DeviceObjectType::Buffer)
		{}

	public:
		virtual ~Buffer() = default;

		inline BufferBindFlags GetBindFlags() const
		{
			return bindFlags;
		}

		virtual void* GetHandle() = 0;

		inline u64 GetBufferSize() const
		{
			return bufferSize;
		}

		virtual void UploadData(const BufferData& data) = 0;

		virtual void ReadData(u8** outData, u64* outDataSize) = 0;

		virtual void Resize(u64 newBufferSize) = 0;

	protected:

		BufferBindFlags bindFlags{};

		u64 bufferSize = 0;

		u64 structureByteStride = 0;

	};
    
} // namespace CE::RHI
