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
        VulkanRenderTargetLayout(VulkanDevice* device, u32 width, u32 height, const RHIRenderTargetLayout& rtLayout);

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

        CE_INLINE bool IsValid() const
        {
            return isValid;
        }

    private:
        bool isValid = false;
        bool hasDepthStencilAttachment = false;

    };

    /// Vulkan Render Pass class
    class VulkanRenderPass;
    
    /// Vulkan Render Target class
    class VulkanRenderTarget : public RHIRenderTarget
    {
    public:
        /// An offscreen render target
        VulkanRenderTarget(VulkanDevice* device, u32 backBufferCount, u32 simultaneousFrameDraws, const VulkanRenderTargetLayout& rtLayout);

        /// A viewport render target
        VulkanRenderTarget(VulkanDevice* device, VulkanViewport* viewport, const VulkanRenderTargetLayout& rtLayout);

        virtual ~VulkanRenderTarget();

        virtual bool IsViewportRenderTarget() override { return isViewportRenderTarget; }

        virtual RHIRenderPass* GetRenderPass() override;

    protected:
        void CreateDepthBuffer();
        void DestroyDepthBuffer();
        void CreateColorBuffers();
        void DestroyColorBuffers();

    private:
        bool isViewportRenderTarget = false;

        VulkanDevice* device = nullptr;
        VulkanRenderTargetLayout rtLayout{};
        VulkanRenderPass* renderPass = nullptr;
        u32 backBufferCount = 0; 
        u32 simultaneousFrameDraws = 0;

        u32 width = 0, height = 0;
    };

} // namespace CE
