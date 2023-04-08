
#include "VulkanQueue.h"

namespace CE
{

	VulkanQueue::VulkanQueue(VulkanDevice* device, u32 familyIndex, u32 queueIndex, VkQueue queue)
		: device(device), familyIndex(familyIndex), queueIndex(queueIndex), queue(queue)
	{

	}

	VulkanQueue::~VulkanQueue()
	{

	}

} // namespace CE
