#include "CoreRHI.h"

namespace CE::RHI
{

    FrameAttachment::FrameAttachment(const AttachmentID& attachmentId, AttachmentLifetimeType lifetime)
        : attachmentId(attachmentId), lifetime(lifetime)
    {
        
    }

	void FrameAttachment::SetResource(RHIResource* resource)
	{
		this->resource = resource;
	}
    
} // namespace CE::RHI
