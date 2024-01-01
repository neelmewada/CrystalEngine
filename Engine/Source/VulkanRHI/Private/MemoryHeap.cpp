#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
	MemoryHeap::MemoryHeap(VulkanDevice* device, const RHI::MemoryHeapDescriptor& desc)
		: device(device)
	{
		debugName = desc.debugName;
		heapType = desc.heapType;
		heapSize = desc.allocationSize;
		usageFlags = desc.usageFlags;
		supportedMemoryTypeBitMask = 0;

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = heapSize;

		switch (heapType)
		{
		case MemoryHeapType::Default:
			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			break;
		case MemoryHeapType::Upload:
			memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			break;
		case MemoryHeapType::ReadBack:
			memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
			break;
		}
		
		VkPhysicalDeviceMemoryProperties memoryProps = device->GetMemoryProperties();

		for (int i = 0; i < memoryProps.memoryTypeCount; i++)
		{
			if ((memoryProps.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
			{
				supportedMemoryTypeBitMask |= (1 << i);
			}
		}

		for (int i = 0; i < memoryProps.memoryTypeCount; i++)
		{
			if (desc.flags == 0 && ((memoryProps.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags))
			{
				allocatedMemoryTypeIndex = i;
				break;
			}
			else if ((desc.flags & (1 << i)) && ((memoryProps.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags))
			{
				allocatedMemoryTypeIndex = i;
				break;
			}
		}

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = heapSize;
		allocInfo.memoryTypeIndex = allocatedMemoryTypeIndex;
		
		auto result = vkAllocateMemory(device->GetHandle(), &allocInfo, nullptr, &allocation);
		if (result != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to allocate memory of size {}: {}", heapSize, debugName);
			return;
		}
	}

	MemoryHeap::~MemoryHeap()
	{
		if (allocation != nullptr)
        {
            vkFreeMemory(device->GetHandle(), allocation, nullptr);
            allocation = nullptr;
        }
	}

	bool MemoryHeap::SupportsOptimalImageTiling()
	{
		if (memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			return false;
		return true;
	}

	bool MemoryHeap::BindBuffer(Buffer* buffer, VkDeviceSize offset)
	{
		auto result = vkBindBufferMemory(device->GetHandle(), buffer->GetBuffer(), allocation, offset);
		if (result != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to allocate buffer in MemoryHeap {}", debugName);
			return false;
		}

		allocatedRanges.Add({ offset, buffer->GetBufferSize() });

		return true;
	}

} // namespace CE::Vulkan
