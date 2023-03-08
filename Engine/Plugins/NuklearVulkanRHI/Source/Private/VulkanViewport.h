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
        virtual ~VulkanViewport();

        virtual RHIRenderTarget* GetRenderTarget() override;

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

        void* windowHandle = nullptr;
        NuklearVulkanRHI* vulkanRHI = nullptr;
        VulkanDevice* device = nullptr;
        VulkanRenderTarget* renderTarget = nullptr;
        VulkanSwapChain* swapChain = nullptr;

        friend struct VulkanRenderTargetLayout;
    };
    
} // namespace CE
