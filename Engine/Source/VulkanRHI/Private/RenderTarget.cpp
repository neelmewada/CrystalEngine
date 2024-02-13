
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

    RenderTarget::RenderTarget(VulkanDevice* device, RenderPass* renderPass)
        : device(device), renderPass(renderPass)
    {
        
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
