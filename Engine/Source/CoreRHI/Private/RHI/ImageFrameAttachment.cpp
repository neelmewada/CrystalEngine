#include "CoreRHI.h"

namespace CE::RHI
{

    ImageFrameAttachment::ImageFrameAttachment(AttachmentID id, const ImageDescriptor& descriptor)
        : FrameAttachment(id, RHI::AttachmentLifetimeType::Transient), descriptor(descriptor)
    {
        
    }

	ImageFrameAttachment::ImageFrameAttachment(AttachmentID id, Texture* image)
		: FrameAttachment(id, RHI::AttachmentLifetimeType::External)
	{
		if (image)
			SetResource(image);
	}

    ImageFrameAttachment::~ImageFrameAttachment()
    {
        
    }
    
} // namespace CE::RHI
