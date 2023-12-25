#pragma once

#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    
    class VulkanRenderPass
    {
    public:
        VulkanRenderPass(VulkanDevice* device, const VulkanRenderTargetLayout& rtLayout);
        ~VulkanRenderPass();

        CE_INLINE VkRenderPass GetHandle()
        {
            return renderPass;
        }

    private:
        VkRenderPass renderPass = nullptr;
        VulkanDevice* device = nullptr;
        VulkanRenderTargetLayout layout{};
    };

} // namespace CE
