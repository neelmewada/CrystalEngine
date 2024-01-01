#pragma once

namespace CE::Vulkan
{
	class Buffer;

	class MemoryHeap final : public RHI::MemoryHeap
	{
	public:
		MemoryHeap(VulkanDevice* device, const RHI::MemoryHeapDescriptor& desc);

		~MemoryHeap();

		bool SupportsOptimalImageTiling();

		bool Allocate(u64 size, u64 alignment);

		bool AllocateBuffer(Buffer* buffer, VkDeviceSize offset);

	private:

		struct Range
		{
			u64 min = 0, max = 0;
		};

		Array<Range> allocatedRanges{};
		Array<Range> freeRanges{};
		VkDeviceMemory allocation{};

		VulkanDevice* device = nullptr;
		VkMemoryPropertyFlags memoryPropertyFlags{};
		u32 supportedMemoryTypeBitMask = 0;
		u32 allocatedMemoryTypeIndex = 0;

	};
    
} // namespace CE::Vulkan
