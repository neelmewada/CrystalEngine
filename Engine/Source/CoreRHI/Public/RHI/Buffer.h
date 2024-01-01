#pragma once

namespace CE::RHI
{
	struct BufferDescriptor
	{
		/// @brief Name used for debugging purposes.
		Name name = "Buffer";
		
		u64 bufferSize = 0;

		u64 structureByteStride = 0;
		
		BufferBindFlags bindFlags{};

		MemoryHeapType defaultHeapType{};
	};

	class MemoryHeap;

	struct ResourceMemoryRequirements
	{
		SIZE_T bufferSize = 0;
		SIZE_T offsetAlignment = 0;
		u32 flags = 0;
	};

	struct ResourceMemoryDescriptor
	{
		RHI::MemoryHeap* memoryHeap = nullptr;
		u64 memoryOffset = 0;
	};

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

		inline u64 GetStructureByteStride() const
		{
			return structureByteStride;
		}

		virtual void* GetHandle() = 0;

		inline u64 GetBufferSize() const
		{
			return bufferSize;
		}

		virtual void UploadData(const BufferData& data) = 0;

		virtual void ReadData(u8** outData, u64* outDataSize) = 0;

	protected:

		/// @brief Name used for debugging purposes.
		Name name{};

		BufferBindFlags bindFlags{};

		u64 bufferSize = 0;

		u64 structureByteStride = 0;

	};
    
} // namespace CE::RHI
