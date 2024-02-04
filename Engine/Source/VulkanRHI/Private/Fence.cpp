#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    
    Fence::Fence(VulkanDevice* device, bool signalled)
        : device(device)
    {
        VkFenceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.flags = 0;
        if (signalled)
            createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        vkCreateFence(device->GetHandle(), &createInfo, nullptr, &fence);
    }

    Fence::~Fence()
    {
        vkDestroyFence(device->GetHandle(), fence, nullptr);
    }

    void Fence::Reset()
    {
        vkResetFences(device->GetHandle(), 1, &fence);
    }

    void Fence::WaitForFence()
    {
        vkWaitForFences(device->GetHandle(), 1, &fence, VK_TRUE, NumericLimits<u64>::Max());
    }

    bool Fence::IsSignalled()
    {
        return vkGetFenceStatus(device->GetHandle(), fence) == VK_SUCCESS;
    }

} // namespace CE::Vulkan
