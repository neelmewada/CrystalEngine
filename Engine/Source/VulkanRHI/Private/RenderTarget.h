#pragma once

namespace CE::Vulkan
{
    
    class RenderTarget : public RHI::RenderTarget
    {
    public:
        RenderTarget(VulkanDevice* device, const RHI::RenderTargetLayout& rtLayout);
        RenderTarget(VulkanDevice* device, const RenderPass::Descriptor& rpDesc);
        RenderTarget(VulkanDevice* device, RenderPass* renderPass);

        RenderTarget* Clone(const Array<RHI::Format>& newColorFormats, RHI::Format depthStencilFormat, u32 subpassSelection) override;

        RenderTarget* Clone(RHI::MultisampleState msaa, const Array<RHI::Format>& newColorFormats, RHI::Format depthStencilFormat, u32 subpassSelection) override;

        void GetAttachmentFormats(Array<RHI::Format>& outColorFormats, RHI::Format& outDepthStencilFormat, u32 subpassSelection) override;

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
