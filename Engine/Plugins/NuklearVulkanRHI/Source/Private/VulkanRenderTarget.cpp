
#include "VulkanRHIPrivate.h"

#include "VulkanRenderPass.h"
#include "VulkanViewport.h"
#include "VulkanSwapChain.h"

namespace CE
{
    // ****************************************************
    // VulkanRenderTargetLayout

    VulkanRenderTargetLayout::VulkanRenderTargetLayout(VulkanDevice* device, u32 width, u32 height, const RHIRenderTargetLayout& rtLayout)
    {
        this->width = width;
        this->height = height;
        this->backBufferCount = rtLayout.backBufferCount;
        this->simultaneousFrameDraws = rtLayout.simultaneousFrameDraws;
        isValid = true;

        colorAttachmentCount = rtLayout.numColorOutputs;
        attachmentDescCount = rtLayout.numColorOutputs;
        hasDepthStencilAttachment = false;

        if (rtLayout.depthStencilFormat != RHIDepthStencilFormat::None) // Depth Stencil Attachment
        {
            hasDepthStencilAttachment = true;

            Array<VkFormat> preferredDepthFormats{};
            if (rtLayout.depthStencilFormat == RHIDepthStencilFormat::Auto || rtLayout.depthStencilFormat == RHIDepthStencilFormat::D32_S8)
                preferredDepthFormats.AddRange({ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT });
            else if (rtLayout.depthStencilFormat == RHIDepthStencilFormat::D24_S8)
                preferredDepthFormats.AddRange({ VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT });
            else if (rtLayout.depthStencilFormat == RHIDepthStencilFormat::D32)
                preferredDepthFormats.AddRange({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT });

            auto depthFormat = device->FindSupportedFormat(preferredDepthFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

            if (depthFormat == VK_FORMAT_UNDEFINED)
            {
                hasDepthStencilAttachment = false;
            }
            else
            {
                // Valid format found
                auto& depthAttachment = attachmentDesc[attachmentDescCount];
                depthAttachment.flags = 0;

                switch (rtLayout.depthStencilFormat)
                {
                case RHIDepthStencilFormat::Auto:
                    depthAttachment.format = depthFormat;
                    break;
                case RHIDepthStencilFormat::D32_S8:
                    depthAttachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
                    break;
                case RHIDepthStencilFormat::D24_S8:
                    depthAttachment.format = VK_FORMAT_D24_UNORM_S8_UINT;
                    break;
                case RHIDepthStencilFormat::D32:
                    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
                    break;
                }
                depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // TODO: Add stencil later
                depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                this->depthFormat = depthAttachment.format;

                depthStencilReference.attachment = attachmentDescCount;
                depthStencilReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                attachmentDescCount++;
            }
        }

        for (int i = 0; i < rtLayout.numColorOutputs; i++) // Color Attachments
        {
            auto& attachment = attachmentDesc[i];
            attachment.flags = 0;

            switch (rtLayout.colorOutputs[i].preferredFormat)
            {
            case RHIColorFormat::Auto:
                attachment.format = device->FindAutoColorFormat().format;
                break;
            case RHIColorFormat::RGBA32:
                if (device->CheckSurfaceFormatSupport(VK_FORMAT_R8G8B8A8_UNORM))
                    attachment.format = VK_FORMAT_R8G8B8A8_UNORM;
                else
                    attachment.format = device->FindAutoColorFormat().format;
                break;
            case RHIColorFormat::BGRA32:
                if (device->CheckSurfaceFormatSupport(VK_FORMAT_B8G8R8A8_UNORM))
                    attachment.format = VK_FORMAT_B8G8R8A8_UNORM;
                else
                    attachment.format = device->FindAutoColorFormat().format;
                break;
            }

            colorFormats[i] = attachment.format;

            attachment.samples = (VkSampleCountFlagBits)rtLayout.colorOutputs[i].sampleCount;

            if (rtLayout.colorOutputs[i].loadAction == RHIRenderPassLoadAction::Clear)
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            else if (rtLayout.colorOutputs[i].loadAction == RHIRenderPassLoadAction::Load)
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            else if (rtLayout.colorOutputs[i].loadAction == RHIRenderPassLoadAction::None)
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

            if (rtLayout.colorOutputs[i].storeAction == RHIRenderPassStoreAction::None)
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            else if (rtLayout.colorOutputs[i].storeAction == RHIRenderPassStoreAction::Store)
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            if (i == rtLayout.presentationRTIndex)
            {
                attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            }
            else
            {
                attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }

            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

            auto& colorReference = colorReferences[i];
            colorReference.attachment = i;
            colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
    }

    VulkanRenderTargetLayout::VulkanRenderTargetLayout(VulkanDevice* device, VulkanViewport* viewport, const RHIRenderTargetLayout& rtLayout)
    {
        this->width = viewport->GetWidth();
        this->height = viewport->GetHeight();
        this->backBufferCount = viewport->GetBackBufferCount();
        this->simultaneousFrameDraws = viewport->GetBackBufferCount();
        isValid = true;

        colorAttachmentCount = rtLayout.numColorOutputs;
        attachmentDescCount = rtLayout.numColorOutputs;
        hasDepthStencilAttachment = false;

        if (rtLayout.depthStencilFormat != RHIDepthStencilFormat::None) // Depth Stencil Attachment
        {
            hasDepthStencilAttachment = true;

            Array<VkFormat> preferredDepthFormats{};
            if (rtLayout.depthStencilFormat == RHIDepthStencilFormat::Auto || rtLayout.depthStencilFormat == RHIDepthStencilFormat::D32_S8)
                preferredDepthFormats.AddRange({ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT });
            else if (rtLayout.depthStencilFormat == RHIDepthStencilFormat::D24_S8)
                preferredDepthFormats.AddRange({ VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT });
            else if (rtLayout.depthStencilFormat == RHIDepthStencilFormat::D32)
                preferredDepthFormats.AddRange({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT });

            //auto depthFormat = viewport->swapChain->depthBufferFormat;
            auto depthFormat = device->FindSupportedFormat(preferredDepthFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

            if (depthFormat == VK_FORMAT_UNDEFINED)
            {
                hasDepthStencilAttachment = false;
            }
            else
            {
                // Valid format found
                auto& depthAttachment = attachmentDesc[attachmentDescCount];
                depthAttachment.flags = 0;

                switch (rtLayout.depthStencilFormat)
                {
                case RHIDepthStencilFormat::Auto:
                    depthAttachment.format = depthFormat;
                    break;
                case RHIDepthStencilFormat::D32_S8:
                    depthAttachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
                    break;
                case RHIDepthStencilFormat::D24_S8:
                    depthAttachment.format = VK_FORMAT_D24_UNORM_S8_UINT;
                    break;
                case RHIDepthStencilFormat::D32:
                    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
                    break;
                }
                depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // TODO: Add stencil later
                depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                this->depthFormat = depthAttachment.format;

                depthStencilReference.attachment = attachmentDescCount;
                depthStencilReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                attachmentDescCount++;
            }
        }

        // TODO: Add color attachments
    }

    // ****************************************************
    // VulkanRenderTarget

    VulkanRenderTarget::VulkanRenderTarget(VulkanDevice* device, const VulkanRenderTargetLayout& rtLayout)
        : isViewportRenderTarget(false)
    {
        this->device = device;
        this->rtLayout = rtLayout;
        this->backBufferCount = rtLayout.backBufferCount;
        this->simultaneousFrameDraws = rtLayout.simultaneousFrameDraws;
        this->width = rtLayout.width;
        this->height = rtLayout.height;

        // Create render pass
        renderPass = new VulkanRenderPass(device, rtLayout);

        // Depth Buffer
        if (rtLayout.HasDepthStencilAttachment())
        {
            CreateDepthBuffer();
        }

        // Color Buffers
        CreateColorBuffers();
    }

    VulkanRenderTarget::VulkanRenderTarget(VulkanDevice* device, VulkanViewport* viewport, const VulkanRenderTargetLayout& rtLayout)
        : isViewportRenderTarget(true)
    {
        this->device = device;
        this->rtLayout = rtLayout;
        this->viewport = viewport;

        // Create render pass
        renderPass = new VulkanRenderPass(device, rtLayout);

        // No need to create any depth & color buffers, handled by VulkanViewport
    }

    VulkanRenderTarget::~VulkanRenderTarget()
    {
        DestroyColorBuffers();
        DestroyDepthBuffer();

        delete renderPass;
    }

    RHIRenderPass* VulkanRenderTarget::GetRenderPass()
    {
        return renderPass;
    }

    u32 VulkanRenderTarget::GetBackBufferCount()
    {
        if (IsViewportRenderTarget())
            return viewport->GetBackBufferCount();
        return backBufferCount;
    }

    u32 VulkanRenderTarget::GetSimultaneousFrameDrawCount()
    {
        if (IsViewportRenderTarget())
            return viewport->GetSimultaneousFrameDrawCount();
        return simultaneousFrameDraws;
    }

    u32 VulkanRenderTarget::GetWidth()
    {
        if (IsViewportRenderTarget())
            return viewport->GetWidth();
        return width;
    }

    u32 VulkanRenderTarget::GetHeight()
    {
        if (IsViewportRenderTarget())
            return viewport->GetHeight();
        return height;
    }

    void VulkanRenderTarget::CreateDepthBuffer()
    {
        auto imageFormat = rtLayout.depthFormat;

    }

    void VulkanRenderTarget::DestroyDepthBuffer()
    {

    }

    void VulkanRenderTarget::CreateColorBuffers()
    {
    }

    void VulkanRenderTarget::DestroyColorBuffers()
    {

    }

} // namespace CE

