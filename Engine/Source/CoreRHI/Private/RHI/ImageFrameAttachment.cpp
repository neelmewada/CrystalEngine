#include "CoreRHI.h"

namespace CE::RHI
{

    ImageFrameAttachment::ImageFrameAttachment(const ImageDescriptor& imageDesc, AttachmentLifetimeType lifetime)
        : FrameAttachment(imageDesc.name, lifetime), descriptor(imageDesc)
    {
        
    }

    ImageFrameAttachment::~ImageFrameAttachment()
    {
        
    }
    
} // namespace CE::RHI
