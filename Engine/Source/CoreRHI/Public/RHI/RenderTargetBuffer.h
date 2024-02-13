#pragma once

namespace CE::RHI
{
    
    struct RenderTargetBufferDescriptor
    {
        RenderTarget* renderTarget = nullptr;
        Array<RHI::Texture*> attachments{};
    };

    class CORERHI_API RenderTargetBuffer : public RHI::RHIResource
    {
    public:
        virtual ~RenderTargetBuffer() = default;

        inline RHI::RenderTarget* GetRenderTarget() const { return renderTarget; }

    protected:

        RenderTargetBuffer() : RHIResource(RHI::ResourceType::RenderTargetBuffer)
        {}

        RenderTarget* renderTarget = nullptr;
    };

} // namespace CE::RHI
