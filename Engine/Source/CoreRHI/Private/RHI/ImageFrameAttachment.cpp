#include "CoreRHI.h"

namespace CE::RHI
{

    ImageFrameAttachment::ImageFrameAttachment(AttachmentID id, const ImageDescriptor& imageDesc)
        : FrameAttachment(id, RHI::AttachmentLifetimeType::Transient), descriptor(imageDesc)
    {
        
    }

	ImageFrameAttachment::ImageFrameAttachment(AttachmentID id, Ptr<Texture> image)
		: FrameAttachment(id, RHI::AttachmentLifetimeType::External)
	{
		if (image)
			SetResource(image);
	}

    ImageFrameAttachment::~ImageFrameAttachment()
    {
        
    }
    
} // namespace CE::RHI
