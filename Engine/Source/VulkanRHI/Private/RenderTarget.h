#pragma once

namespace CE::Vulkan
{
    
    class RenderTarget : public RHI::RenderTarget
    {
    public:
        RenderTarget(VulkanDevice* device, const RHI::RenderTargetLayout& rtLayout);
        RenderTarget(VulkanDevice* device, RenderPass* renderPass);

        ~RenderTarget();

        u32 GetAttachmentCount() const;

        const RenderPass::AttachmentBinding& GetAttachment(u32 index);

        inline RenderPass* GetRenderPass() const { return renderPass; }

    private:

        VulkanDevice* device = nullptr;
        RenderPass* renderPass = nullptr;

        friend class CommandList;
    };

} // namespace CE::Vulkan
