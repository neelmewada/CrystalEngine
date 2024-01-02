#pragma once

namespace CE::RHI
{

    class CORERHI_API ImageFrameAttachment : public FrameAttachment
    {
    public:
        ImageFrameAttachment(const ImageDescriptor& imageDesc, AttachmentLifetimeType lifetime);
        virtual ~ImageFrameAttachment();
        
    private:
        ImageDescriptor descriptor{};
        
    };
    
} // namespace CE::RHI
