#pragma once

#include "NuklearVulkanRHI.h"

#include "vulkan/vulkan.h"

namespace CE
{
    class VulkanDevice;

    class VulkanQueue
    {
    public:
        VulkanQueue(VulkanDevice* device, u32 familyIndex, u32 queueIndex, VkQueue queue);
        ~VulkanQueue();

    private:
        VulkanDevice* device;
        u32 familyIndex;
        u32 queueIndex;
        VkQueue queue;
    };
    
} // namespace CE
