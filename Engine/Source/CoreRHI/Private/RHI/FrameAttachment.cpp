#include "CoreRHI.h"

namespace CE::RHI
{

    FrameAttachment::FrameAttachment(const AttachmentID& attachmentId, AttachmentLifetimeType lifetime)
        : attachmentId(attachmentId), lifetime(lifetime)
    {
        
    }
    
} // namespace CE::RHI
