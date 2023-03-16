
#include "VulkanViewport.h"

#include "VulkanRenderPass.h"
#include "VulkanSwapChain.h"
#include "VulkanTexture.h"

#if PAL_TRAIT_QT_SUPPORTED
#include <QVulkanWindow>
#endif

namespace CE
{
    
    VulkanViewport::VulkanViewport(VulkanRHI* vulkanRHI, VulkanDevice* device, void* windowHandle,
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

#if PAL_TRAIT_QT_SUPPORTED
    VulkanViewport::VulkanViewport(VulkanRHI* vulkanRHI, VulkanDevice* device, void* qtWindowHandle, const RHIRenderTargetLayout& rtLayout)
        : vulkanRHI(vulkanRHI), device(device), windowHandle(qtWindowHandle), isQtWindow(true)
    {
        swapChain = nullptr;
        renderTarget = nullptr;

    }
#endif

    VulkanViewport::~VulkanViewport()
    {
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
            //VkImageView attachments[2] = {};
            //attachments[0] = swapChain->swapChainColorImages[i].imageView;
            //if (swapChain->HasDepthStencilAttachment())
            //{
            //    attachments[1] = swapChain->swapChainDepthImage->GetImageView();
            //}

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

    // - Setters -

    void VulkanViewport::SetClearColor(const Color& color)
    {
        this->clearColor = color;
        this->renderTarget->SetClearColor(renderTarget->rtLayout.presentationRTIndex, color);
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
