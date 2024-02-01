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
		SIZE_T size = 0;
		SIZE_T offsetAlignment = 0;
        /// @brief For internal use only!
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

		//! Returns true if buffer data is directly accessibly on Host (CPU) by mapping & unmapping memory
		inline bool IsHostAccessible() const
		{
			return heapType == RHI::MemoryHeapType::Upload || heapType == RHI::MemoryHeapType::ReadBack;
		}

		virtual void UploadData(const BufferData& data) = 0;

		inline void UploadData(const void* data, u64 dataSize, u64 startOffsetInBuffer = 0)
		{
			RHI::BufferData bufferData{};
			bufferData.data = data;
			bufferData.dataSize = dataSize;
			bufferData.startOffsetInBuffer = startOffsetInBuffer;
			UploadData(bufferData);
		}

		virtual bool Map(u64 offset, u64 size, void** outPtr) = 0;
		virtual bool Unmap() = 0;

		//! Allocates a raw buffer in CPU memory and reads buffer data into it. You are responsible for releasing outData memory block using Memory::Free().
		virtual void ReadData(u8** outData, u64* outDataSize) = 0;

		//! Read data into a pre-allocated memory.
		virtual void ReadData(void* data) = 0;

	protected:

		/// @brief Name used for debugging purposes.
		Name name{};

		BufferBindFlags bindFlags{};

		u64 bufferSize = 0;

		u64 structureByteStride = 0;

		RHI::MemoryHeapType heapType = RHI::MemoryHeapType::Default;
	};
    
} // namespace CE::RHI
