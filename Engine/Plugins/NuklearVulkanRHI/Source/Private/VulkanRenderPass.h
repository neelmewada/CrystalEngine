#pragma once

#include "VulkanRHIPrivate.h"

namespace CE
{
    
    class VulkanRenderPass
    {
    public:
        VulkanRenderPass(VulkanDevice* device, const VulkanRenderTargetLayout& rtLayout);
        ~VulkanRenderPass();

        inline VkRenderPass GetHandle() const
        {
            return renderPass;
        }

        inline const VulkanRenderTargetLayout& GetRTLayout() const
        {
            return rtLayout;
        }

    private:
        VkRenderPass renderPass = nullptr;
        VulkanDevice* device = nullptr;
        VulkanRenderTargetLayout rtLayout{};
    };

} // namespace CE
