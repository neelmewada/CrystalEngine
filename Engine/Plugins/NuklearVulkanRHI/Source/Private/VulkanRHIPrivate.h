#pragma once

#include "CoreMinimal.h"
#include "RHI/RHI.h"

#include "NuklearVulkanRHI.h"

#include "VulkanDevice.h"

namespace CE
{
    class VulkanViewport;

    struct VulkanRenderTargetLayout
    {
    public:
        VulkanRenderTargetLayout() = default;
        ~VulkanRenderTargetLayout() = default;

        /// Offscreen render target layout
        VulkanRenderTargetLayout(VulkanDevice* device, u32 width, u32 height, const RHIRenderPassLayout& rtLayout);

        u32 width = 0, height = 0;

        Color clearColors[RHIMaxSimultaneousRenderOutputs] = {};

        VkFormat colorFormats[RHIMaxSimultaneousRenderOutputs] = {};
        VkFormat depthFormat = {};

        VkAttachmentReference colorReferences[RHIMaxSimultaneousRenderOutputs] = {};
        VkAttachmentReference depthStencilReference = {};

        VkAttachmentDescription attachmentDesc[RHIMaxSimultaneousRenderOutputs + 1]; // countof(ColorAttachments) + 1 depth attachment

        u8 colorAttachmentCount = 0;

        // colorAttachmentCount + 1 depth attachment (if exists)
        u8 attachmentDescCount = 0;

        CE_INLINE bool HasDepthStencilAttachment() const
        {
            return hasDepthStencilAttachment;
        }

    private:
        bool hasDepthStencilAttachment = false;

    };

    /// Vulkan Render Pass class
    class VulkanRenderPass
    {
    public:

    };
    
    /// Vulkan Render Target class
    class VulkanRenderTarget : public RHIRenderTarget
    {
    public:
        /// An offscreen render target
        VulkanRenderTarget(VulkanDevice* device, u32 backBufferCount, u32 simultaneousFrameDraws, const VulkanRenderTargetLayout& rtLayout);

        /// A viewport render target
        VulkanRenderTarget(VulkanDevice* device, VulkanViewport* viewport, const VulkanRenderTargetLayout& rtLayout);

        virtual ~VulkanRenderTarget();

    private:
        bool isViewportRT = false;

        VulkanDevice* device = nullptr;
    };

} // namespace CE
