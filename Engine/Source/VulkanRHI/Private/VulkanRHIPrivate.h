#pragma once

#include "CoreMinimal.h"
#include "CoreApplication.h"
#include "CoreRHI.h"

#include "VulkanRHI.h"
#include "VulkanDevice.h"

#include "vulkan/vulkan.h"

namespace CE
{
    class VulkanViewport;

    struct VulkanRenderTargetLayout
    {
    public:
        VulkanRenderTargetLayout() = default;
        ~VulkanRenderTargetLayout() = default;

        /// Offscreen render target layout
        VulkanRenderTargetLayout(VulkanDevice* device, u32 width, u32 height, const RHI::RenderTargetLayout& rtLayout);

        /// Viewport render target layout
        VulkanRenderTargetLayout(VulkanDevice* device, VulkanViewport* viewport, const RHI::RenderTargetLayout& rtLayout);

        u32 width = 0, height = 0;
        u32 presentationRTIndex = -1;

        Color clearColors[RHI::MaxSimultaneousRenderOutputs] = {};

        VkFormat colorFormats[RHI::MaxSimultaneousRenderOutputs] = {};
        VkFormat depthFormat = {};

        VkAttachmentReference colorReferences[RHI::MaxSimultaneousRenderOutputs] = {};
        VkAttachmentReference depthStencilReference = {};

        VkAttachmentDescription attachmentDesc[RHI::MaxSimultaneousRenderOutputs + 1]; // countof(ColorAttachments) + 1 depth attachment

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
        VulkanFrameBuffer(VulkanDevice* device, VulkanSwapChain* swapChain, u32 swapChainImageIndex, VulkanRenderTarget* renderTarget);
        VulkanFrameBuffer(VulkanDevice* device, VkImageView attachments[RHI::MaxSimultaneousRenderOutputs + 1], VulkanRenderTarget* renderTarget);

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
        List<VulkanTexture*> textures{};
        List<VkSampler> samplers{};
        VulkanFrameBuffer* framebuffer = nullptr;
    };
    
    /// Vulkan Render Target class
    class VulkanRenderTarget : public RHI::RenderTarget
    {
    public:
        /// An offscreen render target
        VulkanRenderTarget(VulkanDevice* device, const VulkanRenderTargetLayout& rtLayout);

        /// A viewport render target
        VulkanRenderTarget(VulkanDevice* device, VulkanViewport* viewport, const VulkanRenderTargetLayout& rtLayout);

        virtual ~VulkanRenderTarget();

        virtual bool IsViewportRenderTarget() override { return isViewportRenderTarget; }

        virtual RHI::RenderPass* GetRenderPass() override;

        virtual void SetClearColor(u32 colorTargetIndex, const Color& color) override;

        // - Getters -

        CE_INLINE VulkanRenderPass* GetVulkanRenderPass() { return renderPass; }

        CE_INLINE VulkanViewport* GetViewport() { return viewport; }

        VkRenderPass GetVulkanRenderPassHandle() const;

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
        
        Color clearColors[RHI::MaxSimultaneousRenderOutputs] = {};
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
        friend class VulkanRHI;
    };

    /*
    *   Graphics Command List
    */

    class VulkanGraphicsCommandList : public RHI::GraphicsCommandList, public ApplicationMessageHandler
    {
    public:
        VulkanGraphicsCommandList(VulkanRHI* vulkanRHI, VulkanDevice* device, VulkanViewport* viewport);
        VulkanGraphicsCommandList(VulkanRHI* vulkanRHI, VulkanDevice* device, VulkanRenderTarget* renderTarget);
        virtual ~VulkanGraphicsCommandList();

        // - Rendering API -

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

        void ProcessNativeEvents(void* nativeEvent) override;

        // - ImGui API -

        bool InitImGui(RHI::FontPreloadConfig* preloadFonts = nullptr) override;

        void ShutdownImGui() override;

        virtual void ImGuiNewFrame() override;
        virtual void ImGuiRender() override;

        void ImGuiPlatformUpdate() override;

    protected:
        void CreateSyncObjects();
        void DestroySyncObjects();

    private:
        VulkanRHI* vulkanRHI = nullptr;
        VulkanDevice* device = nullptr;
        VulkanRenderTarget* renderTarget = nullptr;
        VulkanViewport* viewport = nullptr;

        u32 numCommandBuffers = 1; // = BackBufferCount
        u32 simultaneousFrameDraws = 1;

        u32 currentImageIndex = 0;

        List<VkCommandBuffer> commandBuffers{};

        List<VkFence> renderFinishedFence{}; // Size = NumCommandBuffers
        List<VkSemaphore> renderFinishedSemaphore{}; // Size = NumCommandBuffers

        // ImGui
        b8 isImGuiEnabled = false;
        VkDescriptorPool imGuiDescriptorPool;

        friend class VulkanRHI;

    };

} // namespace CE
