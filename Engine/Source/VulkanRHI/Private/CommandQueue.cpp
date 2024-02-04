
#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

	CommandQueue::CommandQueue(VulkanDevice* device, 
		u32 familyIndex, u32 queueIndex, 
		RHI::HardwareQueueClassMask queueMask, 
		VkQueue queue, bool presentSupported)
		: device(device)
		, familyIndex(familyIndex)
		, queueIndex(queueIndex)
		, queue(queue)
		, presentSupported(presentSupported)
	{
		this->queueMask = queueMask;
	}

	CommandQueue::~CommandQueue()
	{
		
	}

	bool CommandQueue::Execute(u32 count, RHI::CommandList** commandLists, RHI::Fence* fence)
	{
		static List<VkCommandBuffer> commandBuffers{};
		if (count > commandBuffers.GetSize())
			commandBuffers.Resize(count);

		for (int i = 0; i < count; i++)
		{
			if (commandLists[i] == nullptr)
				return false;

			commandBuffers[i] = ((Vulkan::CommandList*)commandLists[i])->GetCommandBuffer();
		}

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = count;
		submitInfo.pCommandBuffers = commandBuffers.GetData();

		VkFence vkFence = nullptr;
		if (fence != nullptr)
		{
			vkFence = ((Vulkan::Fence*)fence)->GetHandle();
		}
		
		auto result = vkQueueSubmit(queue, 1, &submitInfo, vkFence);

		return result == VK_SUCCESS;
	}

} // namespace CE
