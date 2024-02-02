#pragma once

namespace CE::RHI
{
	struct TransientMemoryAllocation
	{
		ResourceMemoryRequirements imagePool{};
		ResourceMemoryRequirements bufferPool{};
	};
    
	class CORERHI_API TransientMemoryPool final
	{
	public:

		TransientMemoryPool();
		virtual ~TransientMemoryPool();

		void AllocateMemoryPool(const TransientMemoryAllocation& allocInfo, bool* bufferPoolRecreated, bool* imagePoolRecreated, bool allowShrink = false);

		RHI::Buffer* AllocateBuffer(const RHI::BufferDescriptor& descriptor, u64 memoryOffset);
		RHI::Texture* AllocateImage(const RHI::ImageDescriptor& descriptor, u64 memoryOffset);

		inline const Array<RHI::Buffer*>& GetAllocatedBuffers() const
		{
			return allocatedBuffers;
		}

		inline const Array<RHI::Texture*>& GetAllocatedImages() const
		{
			return allocatedImages;
		}

		inline RHI::MemoryHeap* GetBufferPool() const { return bufferPool; }
		inline RHI::MemoryHeap* GetImagePool() const { return imagePool; }

	private:

		Array<RHI::Buffer*> allocatedBuffers{};
		Array<RHI::Texture*> allocatedImages{};

		RHI::MemoryHeap* bufferPool = nullptr;
		RHI::MemoryHeap* imagePool = nullptr;

	};

} // namespace CE::RHI
