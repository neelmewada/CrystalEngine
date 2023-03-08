
#include "VulkanViewport.h"

#include "VulkanRenderPass.h"
#include "VulkanSwapChain.h"

namespace CE
{
    
    VulkanViewport::VulkanViewport(NuklearVulkanRHI* vulkanRHI, VulkanDevice* device, void* windowHandle,
        u32 width, u32 height,
        bool isFullscreen, const RHIRenderTargetLayout& rtLayout)
        : vulkanRHI(vulkanRHI), device(device), windowHandle(windowHandle), isFullscreen(isFullscreen)
    {
        auto backBufferCount = rtLayout.backBufferCount;
        auto simultaneousFrameDraws = rtLayout.simultaneousFrameDraws;

        if (simultaneousFrameDraws > backBufferCount - 1)
            simultaneousFrameDraws = backBufferCount - 1;

        // - Swap Chain -
        s32 presentationIndex = Math::Max(rtLayout.presentationRTIndex, 0);

        swapChain = new VulkanSwapChain(vulkanRHI, windowHandle, device, backBufferCount, simultaneousFrameDraws, 
            width, height, isFullscreen, 
            rtLayout.colorOutputs[presentationIndex].preferredFormat, rtLayout.depthStencilFormat);

        // - Render Target -
        renderTarget = new VulkanRenderTarget(device, this, VulkanRenderTargetLayout(device, this, rtLayout));

        // - Frame Buffers -
        CreateFrameBuffers();

        // - Sync Objects -
        CreateSyncObjects();

        CE_LOG(Info, All, "Vulkan Viewport created");
    }

    VulkanViewport::~VulkanViewport()
    {
        // - Render Target -
        delete renderTarget;

        // - Swap Chain -
        delete swapChain;

        CE_LOG(Info, All, "Vulkan Viewport destroyed");
    }

    void VulkanViewport::CreateFrameBuffers()
    {

    }

    void VulkanViewport::CreateSyncObjects()
    {

    }

    // - Setters -

    void VulkanViewport::SetClearColor(const Color& color)
    {
        this->clearColor = color;
    }

    // - Getters -

    RHIRenderTarget* VulkanViewport::GetRenderTarget()
    {
        return renderTarget;
    }

    u32 VulkanViewport::GetBackBufferCount()
    {
        return swapChain->GetBackBufferCount();
    }

    u32 VulkanViewport::GetSimultaneousFrameDrawCount()
    {
        return swapChain->GetSimultaneousFrameDraws();
    }

    u32 VulkanViewport::GetWidth()
    {
        return swapChain->GetWidth();
    }

    u32 VulkanViewport::GetHeight()
    {
        return swapChain->GetHeight();
    }

} // namespace CE
