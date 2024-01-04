#pragma once

namespace CE::RHI
{

    class CORERHI_API ImageFrameAttachment : public FrameAttachment
    {
    public:

        //! Create a transient image attachment
        ImageFrameAttachment(AttachmentID id, const ImageDescriptor& imageDesc);

		//! Create an external image attachment 
		ImageFrameAttachment(AttachmentID id, Ptr<Texture> image);

        virtual ~ImageFrameAttachment();
        
    private:
        ImageDescriptor descriptor{};
        
    };
    
} // namespace CE::RHI
