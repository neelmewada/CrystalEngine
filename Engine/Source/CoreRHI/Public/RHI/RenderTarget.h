#pragma once

namespace CE::RHI
{

    struct RenderAttachmentLayout
    {
        AttachmentID attachmentId{};
        ScopeAttachmentUsage attachmentUsage{};
        RHI::Format format{};
        RHI::MultisampleState multisampleState{};

        AttachmentLoadAction loadAction{};
        AttachmentStoreAction storeAction{};

        AttachmentLoadAction loadActionStencil{};
        AttachmentStoreAction storeActionStencil{};
    };

    struct RenderTargetLayout
    {
        FixedArray<RenderAttachmentLayout, RHI::Limits::Pipeline::MaxRenderAttachmentCount> attachmentLayouts{};
    };

    class CORERHI_API RenderTarget : RHI::RHIResource
    {
    public:

        virtual ~RenderTarget() {}

    protected:
        RenderTarget() : RHI::RHIResource(RHI::ResourceType::RenderTarget)
        {}
        
    };
    
} // namespace CE::RHI
