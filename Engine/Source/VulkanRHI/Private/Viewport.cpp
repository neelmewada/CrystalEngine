
#include "VulkanRHIPrivate.h"

#include "PAL/Common/VulkanPlatform.h"

#if PAL_TRAIT_QT_SUPPORTED
#include <QVulkanWindow>
#endif

namespace CE::Vulkan
{
    
    VulkanViewport::VulkanViewport(VulkanRHI* vulkanRHI, VulkanDevice* device, PlatformWindow* windowHandle,
        u32 width, u32 height,
        bool isFullscreen, const RHI::RenderTargetLayout& rtLayout)
        : vulkanRHI(vulkanRHI), device(device), windowHandle(windowHandle), isFullscreen(isFullscreen)
    {
        auto backBufferCount = rtLayout.backBufferCount;
        auto simultaneousFrameDraws = rtLayout.simultaneousFrameDraws;
        
        windowResizeDelegateHandle =
            PlatformApplication::Get()->onWindowResized.AddDelegateInstance(MemberDelegate(&VulkanViewport::OnWindowResized, this));

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
        PlatformApplication::Get()->onWindowResized.RemoveDelegateInstance(windowResizeDelegateHandle);
        
        // - Sync Objects -
        for (int i = 0; i < imageAcquiredSemaphore.GetSize(); ++i)
        {
            vkDestroySemaphore(device->GetHandle(), imageAcquiredSemaphore[i], nullptr);
        }
        imageAcquiredSemaphore.Clear();

        // - Frame Buffers -
        for (int i = 0; i < frameBuffers.GetSize(); i++)
        {
            delete frameBuffers[i];
        }
        frameBuffers.Clear();

        // - Render Target -
        delete renderTarget;

        // - Swap Chain -
        delete swapChain;

        CE_LOG(Info, All, "Vulkan Viewport destroyed");
    }

    void VulkanViewport::CreateFrameBuffers()
    {
        frameBuffers.Resize(swapChain->GetBackBufferCount());

        for (int i = 0; i < frameBuffers.GetSize(); i++)
        {
            frameBuffers[i] = new VulkanFrameBuffer(device, swapChain, i, renderTarget);
        }
    }

    void VulkanViewport::CreateSyncObjects()
    {
        VkSemaphoreCreateInfo semaphoreCI = {};
        semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        imageAcquiredSemaphore.Resize(GetBackBufferCount());

        for (int i = 0; i < imageAcquiredSemaphore.GetSize(); ++i)
        {
            if (vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &imageAcquiredSemaphore[i]) != VK_SUCCESS)
            {
                CE_LOG(Error, All, "Failed to create sync objects for Vulkan Viewport");
                return;
            }
        }
    }

    void VulkanViewport::OnWindowResized(PlatformWindow* window, u32 width, u32 height)
    {
        Rebuild();
    }

    // - Setters -

    void VulkanViewport::SetClearColor(const Color& color)
    {
        this->clearColor = color;
        this->renderTarget->SetClearColor(renderTarget->rtLayout.presentationRTIndex, color);
    }

    // - Getters -

    RHI::RenderTarget* VulkanViewport::GetRenderTarget()
    {
        return renderTarget;
    }

    void VulkanViewport::Rebuild()
    {
        VulkanPlatform::GetDrawableWindowSize(windowHandle, &swapChain->width, &swapChain->height);

        swapChain->RebuildSwapChain();

        for (int i = 0; i < frameBuffers.GetSize(); i++)
        {
            delete frameBuffers[i];
        }
        frameBuffers.Clear();

        CreateFrameBuffers();
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
