#pragma once

#include "VulkanRHIPrivate.h"

namespace CE
{
    
    class VulkanRenderPass : public RHIRenderPass
    {
    public:
        VulkanRenderPass(VulkanDevice* device, const VulkanRenderTargetLayout& rtLayout);
        ~VulkanRenderPass();

    private:
        VkRenderPass renderPass = nullptr;
        VulkanDevice* device = nullptr;
        VulkanRenderTargetLayout layout{};
    };

} // namespace CE
