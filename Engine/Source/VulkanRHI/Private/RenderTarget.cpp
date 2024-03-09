
#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

    RenderTarget::RenderTarget(VulkanDevice* device, const RHI::RenderTargetLayout& rtLayout)
        : device(device)
    {
        RenderPass::Descriptor desc{};
        RenderPass::BuildDescriptor(rtLayout, desc);
        renderPass = device->GetRenderPassCache()->FindOrCreate(desc);
    }

    RenderTarget::RenderTarget(VulkanDevice* device, const RenderPass::Descriptor& rpDesc)
        : device(device)
    {
        renderPass = device->GetRenderPassCache()->FindOrCreate(rpDesc);
    }

    RenderTarget::RenderTarget(VulkanDevice* device, RenderPass* renderPass)
        : device(device), renderPass(renderPass)
    {
        
    }

    RenderTarget* RenderTarget::Clone(const Array<RHI::Format>& newColorFormats, RHI::Format depthStencilFormat, u32 subpassSelection)
    {
        if (renderPass == nullptr)
            return nullptr;

        RenderPass::Descriptor rpDesc = renderPass->GetDescriptor();
        if (subpassSelection >= rpDesc.subpasses.GetSize())
            return nullptr;
        
        const auto& subpass = rpDesc.subpasses[subpassSelection];
        
        for (int i = 0; i < subpass.colorAttachments.GetSize(); i++)
        {
            if (i >= newColorFormats.GetSize())
                break;
            if (newColorFormats[i] == RHI::Format::Undefined)
                continue;

            u32 attachmentIndex = subpass.colorAttachments[i].attachmentIndex;
            rpDesc.attachments[attachmentIndex].format = newColorFormats[i];
        }

        if (subpass.depthStencilAttachment.GetSize() > 0 && depthStencilFormat != RHI::Format::Undefined)
        {
            u32 attachmentIndex = subpass.depthStencilAttachment[0].attachmentIndex;
            rpDesc.attachments[attachmentIndex].format = depthStencilFormat;
        }

        return new RenderTarget(device, rpDesc);
    }

    RenderTarget* RenderTarget::Clone(MultisampleState msaa, const Array<RHI::Format>& newColorFormats, RHI::Format depthStencilFormat, u32 subpassSelection)
    {
        if (renderPass == nullptr)
            return nullptr;

        RenderPass::Descriptor rpDesc = renderPass->GetDescriptor();
        if (subpassSelection >= rpDesc.subpasses.GetSize())
            return nullptr;

        const auto& subpass = rpDesc.subpasses[subpassSelection];

        for (int i = 0; i < subpass.colorAttachments.GetSize(); i++)
        {
            if (i >= newColorFormats.GetSize())
                break;
            if (newColorFormats[i] == RHI::Format::Undefined)
                continue;

            u32 attachmentIndex = subpass.colorAttachments[i].attachmentIndex;
            rpDesc.attachments[attachmentIndex].format = newColorFormats[i];
            rpDesc.attachments[attachmentIndex].multisampleState = msaa;
        }

        if (subpass.depthStencilAttachment.GetSize() > 0 && depthStencilFormat != RHI::Format::Undefined)
        {
            u32 attachmentIndex = subpass.depthStencilAttachment[0].attachmentIndex;
            rpDesc.attachments[attachmentIndex].format = depthStencilFormat;
            rpDesc.attachments[attachmentIndex].multisampleState = msaa;
        }

        return new RenderTarget(device, rpDesc);
    }

    void RenderTarget::GetAttachmentFormats(Array<RHI::Format>& outColorFormats, RHI::Format& outDepthStencilFormat, u32 subpassSelection)
    {
        outColorFormats.Clear();
        outDepthStencilFormat = Format::Undefined;

        if (renderPass == nullptr)
            return;

        RenderPass::Descriptor rpDesc = renderPass->GetDescriptor();
        if (subpassSelection >= rpDesc.subpasses.GetSize())
            return;

        const auto& subpass = rpDesc.subpasses[subpassSelection];

        for (int i = 0; i < subpass.colorAttachments.GetSize(); i++)
        {
            u32 attachmentIndex = subpass.colorAttachments[i].attachmentIndex;
            outColorFormats.Add(rpDesc.attachments[attachmentIndex].format);
        }

        if (subpass.depthStencilAttachment.GetSize() > 0)
        {
            u32 attachmentIndex = subpass.depthStencilAttachment[0].attachmentIndex;
            outDepthStencilFormat = rpDesc.attachments[attachmentIndex].format;
        }
    }

    RenderTarget::~RenderTarget()
    {
        // Never destroy a render pass manually, it is cached and destroyed automatically when RHI is shut down.
    }

    u32 RenderTarget::GetAttachmentCount() const
    {
        return renderPass->GetDescriptor().attachments.GetSize();
    }

    const RenderPass::AttachmentBinding& RenderTarget::GetAttachment(u32 index)
    {
        return renderPass->GetDescriptor().attachments[index];
    }
    
} // namespace CE::Vulkan
