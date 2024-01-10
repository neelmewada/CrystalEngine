#include "CoreRHI.h"

namespace CE::RHI
{

    FrameAttachment::FrameAttachment(const AttachmentID& attachmentId, AttachmentLifetimeType lifetime)
        : attachmentId(attachmentId), lifetime(lifetime)
    {
        
    }

	FrameAttachment::~FrameAttachment()
	{
        // Destroy transient resource
		if (lifetime == RHI::AttachmentLifetimeType::Transient && resource)
		{
			delete resource;
		}
	}

	void FrameAttachment::SetResource(RHIResource* resource)
	{
		this->resource = resource;
	}
    
} // namespace CE::RHI
