#include "CoreRHI.h"

namespace CE::RHI
{

    FrameAttachment::FrameAttachment(const AttachmentID& attachmentId, AttachmentLifetimeType lifetime)
        : attachmentId(attachmentId), lifetime(lifetime)
    {
        
    }

	FrameAttachment::~FrameAttachment()
	{
		if (lifetime != RHI::AttachmentLifetimeType::External && resource)
		{
			delete resource;
		}
	}

	void FrameAttachment::SetResource(RHIResource* resource)
	{
		this->resource = resource;
	}
    
} // namespace CE::RHI
