#pragma once

#include "VulkanRHIPrivate.h"

namespace CE
{

    class VulkanViewport : public RHIViewport
    {
    public:
        VulkanViewport(NuklearVulkanRHI* vulkanRHI, VulkanDevice* device, void* windowHandle,
            u32 width, u32 height,
            bool isFullscreen, const RHIRenderTargetLayout& rtLayout);
#if PAL_TRAIT_QT_SUPPORTED
        VulkanViewport(NuklearVulkanRHI* vulkanRHI, VulkanDevice* device, void* qtWindowHandle, const RHIRenderTargetLayout& rtLayout);
#endif
        virtual ~VulkanViewport();

        virtual RHIRenderTarget* GetRenderTarget() override;

        VulkanSwapChain* GetSwapChain() { return swapChain; }

        // - Setters -

        virtual void SetClearColor(const Color& color) override;

        // - Getters -

        u32 GetBackBufferCount();
        u32 GetSimultaneousFrameDrawCount();
        u32 GetWidth();
        u32 GetHeight();

    protected:

        void CreateFrameBuffers();
        void CreateSyncObjects();

    private:
        Color clearColor{};
        bool isFullscreen = false;
        bool isQtWindow = false;

        void* windowHandle = nullptr;
        NuklearVulkanRHI* vulkanRHI = nullptr;
        VulkanDevice* device = nullptr;
        VulkanRenderTarget* renderTarget = nullptr;
        VulkanSwapChain* swapChain = nullptr;

        Array<VulkanFrameBuffer*> frameBuffers{};

        // Sync Objects (Per simultaneous frame)
        // Count = number of frames that are rendered simultaneously
        Vector<VkSemaphore> imageAcquiredSemaphore{};

        u32 currentDrawFrameIndex = 0;
        u32 currentImageIndex = 0;

        friend struct VulkanRenderTargetLayout;
        friend class VulkanGraphicsCommandList;
        friend class NuklearVulkanRHI;
    };
    
} // namespace CE
