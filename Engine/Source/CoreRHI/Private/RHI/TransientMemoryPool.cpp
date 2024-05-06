#include "CoreRHI.h"

namespace CE::RHI
{
    TransientMemoryPool::TransientMemoryPool()
    {
        
    }

	TransientMemoryPool::~TransientMemoryPool()
	{
		for (auto buffer : allocatedBuffers)
		{
			RHI::gDynamicRHI->DestroyBuffer(buffer);
		}
		allocatedBuffers.Clear();

		for (auto image : allocatedImages)
		{
			RHI::gDynamicRHI->DestroyTexture(image);
		}
		allocatedImages.Clear();

		if (bufferPool)
			delete bufferPool;
		bufferPool = nullptr;
		if (imagePool)
			delete imagePool;
		imagePool = nullptr;
	}

    void TransientMemoryPool::AllocateMemoryPool(const TransientMemoryAllocation& allocInfo, 
		bool* bufferPoolRecreated, bool* imagePoolRecreated, bool allowShrink)
    {
		MemoryHeapDescriptor bufferHeapDesc{};
		bufferHeapDesc.allocationSize = allocInfo.bufferPool.size;
		bufferHeapDesc.flags = allocInfo.bufferPool.flags;
		bufferHeapDesc.debugName = "Transient Buffer Pool";
		bufferHeapDesc.usageFlags = RHI::MemoryHeapUsageFlags::Buffer;
		bufferHeapDesc.heapType = RHI::MemoryHeapType::Default;

		for (auto buffer : allocatedBuffers)
		{
			RHI::gDynamicRHI->DestroyBuffer(buffer);
		}
		allocatedBuffers.Clear();

		if (bufferPool == nullptr || bufferPool->GetHeapSize() < bufferHeapDesc.allocationSize || allowShrink)
		{
			if (bufferPool)
			{
				RHI::gDynamicRHI->FreeMemoryHeap(bufferPool);
			}

			*bufferPoolRecreated = true;
			if (bufferHeapDesc.allocationSize > 0)
				bufferPool = RHI::gDynamicRHI->AllocateMemoryHeap(bufferHeapDesc);
		}

		MemoryHeapDescriptor imageHeapDesc{};
		imageHeapDesc.allocationSize = allocInfo.imagePool.size;
		imageHeapDesc.flags = allocInfo.imagePool.flags;
		imageHeapDesc.debugName = "Transient Image Pool";
		imageHeapDesc.usageFlags = RHI::MemoryHeapUsageFlags::Texture;
		imageHeapDesc.heapType = RHI::MemoryHeapType::Default;
		
		for (auto image : allocatedImages)
		{
			RHI::gDynamicRHI->DestroyTexture(image);
		}
		allocatedImages.Clear();

		if (imagePool == nullptr || imagePool->GetHeapSize() < imageHeapDesc.allocationSize || allowShrink)
		{
			if (imagePool)
			{
				RHI::gDynamicRHI->FreeMemoryHeap(imagePool);
				imagePool = nullptr;
			}

			*imagePoolRecreated = true;

			if (imageHeapDesc.allocationSize > 0)
				imagePool = RHI::gDynamicRHI->AllocateMemoryHeap(imageHeapDesc);
		}
    }

	RHI::Buffer* TransientMemoryPool::AllocateBuffer(const RHI::BufferDescriptor& descriptor, u64 memoryOffset)
	{
		ResourceMemoryDescriptor memoryDesc{};
		memoryDesc.memoryHeap = bufferPool;
		memoryDesc.memoryOffset = memoryOffset;
		RHI::Buffer* buffer = RHI::gDynamicRHI->CreateBuffer(descriptor, memoryDesc);
		allocatedBuffers.Add(buffer);
		return buffer;
	}

	RHI::Texture* TransientMemoryPool::AllocateImage(const RHI::ImageDescriptor& descriptor, u64 memoryOffset)
	{
		ResourceMemoryDescriptor memoryDesc{};
		memoryDesc.memoryHeap = imagePool;
		memoryDesc.memoryOffset = memoryOffset;
		RHI::Texture* image = RHI::gDynamicRHI->CreateTexture(descriptor, memoryDesc);
		allocatedImages.Add(image);
		return image;
	}

} // namespace CE::RHI
