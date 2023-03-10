#pragma once

#include "CoreMinimal.h"
#include "RHI/RHI.h"

#include "NuklearVulkanRHI.h"

#include "vulkan/vulkan.h"

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

        /// Viewport render target layout
        VulkanRenderTargetLayout(VulkanDevice* device, VulkanViewport* viewport, const RHIRenderTargetLayout& rtLayout);

        u32 width = 0, height = 0;
        u32 presentationRTIndex = -1;

        Color clearColors[RHIMaxSimultaneousRenderOutputs] = {};

        VkFormat colorFormats[RHIMaxSimultaneousRenderOutputs] = {};
        VkFormat depthFormat = {};

        VkAttachmentReference colorReferences[RHIMaxSimultaneousRenderOutputs] = {};
        VkAttachmentReference depthStencilReference = {};

        VkAttachmentDescription attachmentDesc[RHIMaxSimultaneousRenderOutputs + 1]; // countof(ColorAttachments) + 1 depth attachment

        u8 colorAttachmentCount = 0;

        // colorAttachmentCount + 1 depth attachment (if exists)
        u8 attachmentDescCount = 0;

        u32 backBufferCount = 2, simultaneousFrameDraws = 1;

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

    /*
    *   Vulkan Frame Buffer
    */

    class VulkanSwapChain;
    class VulkanRenderTarget;
    class VulkanTexture;

    class VulkanFrameBuffer
    {
    public:
        VulkanFrameBuffer(VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderTarget* renderTarget);
        VulkanFrameBuffer(VulkanDevice* device, VkImageView attachments[RHIMaxSimultaneousRenderOutputs + 1], VulkanRenderTarget* renderTarget);

        ~VulkanFrameBuffer();

        inline VkFramebuffer GetHandle() const
        {
            return frameBuffer;
        }

    private:
        VulkanDevice* device = nullptr;
        VkFramebuffer frameBuffer = nullptr;
    };

    /// Vulkan Render Pass class
    class VulkanRenderPass;

    struct VulkanFrame
    {
        Vector<VulkanTexture*> textures{};
        Vector<VkSampler> samplers{};
        VulkanFrameBuffer* framebuffer = nullptr;
    };
    
    /// Vulkan Render Target class
    class VulkanRenderTarget : public RHIRenderTarget
    {
    public:
        /// An offscreen render target
        VulkanRenderTarget(VulkanDevice* device, const VulkanRenderTargetLayout& rtLayout);

        /// A viewport render target
        VulkanRenderTarget(VulkanDevice* device, VulkanViewport* viewport, const VulkanRenderTargetLayout& rtLayout);

        virtual ~VulkanRenderTarget();

        virtual bool IsViewportRenderTarget() override { return isViewportRenderTarget; }

        virtual RHIRenderPass* GetRenderPass() override;

        virtual void SetClearColor(u32 colorTargetIndex, const Color& color) override;

        // - Getters -

        CE_INLINE VulkanRenderPass* GetVulkanRenderPass() { return renderPass; }

        CE_INLINE VulkanViewport* GetViewport() { return viewport; }

        u32 GetBackBufferCount();

        u32 GetSimultaneousFrameDrawCount();

        u32 GetWidth();

        u32 GetHeight();

        u32 GetColorAttachmentCount()
        {
            return rtLayout.colorAttachmentCount;
        }

        u32 GetTotalAttachmentCount()
        {
            return rtLayout.attachmentDescCount;
        }

        bool HasDepthStencilAttachment() const
        {
            return rtLayout.HasDepthStencilAttachment();
        }

    protected:
        void CreateDepthBuffer();
        void DestroyDepthBuffer();
        void CreateColorBuffers();
        void DestroyColorBuffers();

    private:
        bool isViewportRenderTarget = false;
        VulkanViewport* viewport = nullptr;
        bool isFresh = true;
        
        Color clearColors[RHIMaxSimultaneousRenderOutputs] = {};
        VulkanDevice* device = nullptr;
        VulkanRenderTargetLayout rtLayout{};
        VulkanRenderPass* renderPass = nullptr;
        u32 backBufferCount = 0; 
        u32 simultaneousFrameDraws = 0;

        u32 width = 0, height = 0;

        u32 currentDrawFrameIndex = 0;
        u32 currentImageIndex = 0;

        // Offscreen render targets
        Array<VulkanFrame> colorFrames{};
        VulkanFrame depthFrame{};

        friend class VulkanViewport;
        friend class VulkanGraphicsCommandList;
        friend class VulkanFrameBuffer;
        friend class NuklearVulkanRHI;
    };

    /*
    *   Graphics Command List
    */

    class VulkanGraphicsCommandList : public RHIGraphicsCommandList
    {
    public:
        VulkanGraphicsCommandList(NuklearVulkanRHI* vulkanRHI, VulkanDevice* device, VulkanViewport* viewport);
        VulkanGraphicsCommandList(NuklearVulkanRHI* vulkanRHI, VulkanDevice* device, VulkanRenderTarget* renderTarget);
        virtual ~VulkanGraphicsCommandList();

        virtual void Begin() override;
        virtual void End() override;

        CE_INLINE bool IsViewportTarget()
        {
            return viewport != nullptr;
        }

        CE_INLINE VulkanViewport* GetViewport()
        {
            return viewport;
        }

        CE_INLINE VulkanRenderTarget* GetRenderTarget()
        {
            return renderTarget;
        }

    protected:
        void CreateSyncObjects();
        void DestroySyncObjects();

    private:
        NuklearVulkanRHI* vulkanRHI = nullptr;
        VulkanDevice* device = nullptr;
        VulkanRenderTarget* renderTarget = nullptr;
        VulkanViewport* viewport = nullptr;

        u32 numCommandBuffers = 1; // = BackBufferCount
        u32 simultaneousFrameDraws = 1;

        u32 currentImageIndex = 0;

        Vector<VkCommandBuffer> commandBuffers{};

        Vector<VkFence> renderFinishedFence{}; // Size = NumCommandBuffers
        Vector<VkSemaphore> renderFinishedSemaphore{}; // Size = NumCommandBuffers

        friend class NuklearVulkanRHI;
    };

} // namespace CE
