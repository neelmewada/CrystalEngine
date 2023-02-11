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

        CE_INLINE VkQueue GetHandle() const
        {
            return queue;
        }

        CE_INLINE u32 GetFamilyIndex() const
        {
            return familyIndex;
        }

        CE_INLINE u32 GetQueueIndex() const
        {
            return queueIndex;
        }

    private:
        VulkanDevice* device;
        u32 familyIndex;
        u32 queueIndex;
        VkQueue queue;
    };
    
} // namespace CE
