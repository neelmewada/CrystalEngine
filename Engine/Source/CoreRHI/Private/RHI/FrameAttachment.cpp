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
		if (lifetime == RHI::AttachmentLifetimeType::Transient)
		{
			for (int i = 0; i < resources.GetSize(); i++)
			{
				if (resources[i])
					delete resources[i];
				resources[i] = nullptr;
			}
		}
	}

	void FrameAttachment::SetResource(RHIResource* resource)
	{
		for (int i = 0; i < resources.GetSize(); i++)
		{
			resources[i] = resource;
		}
	}

	void FrameAttachment::SetResource(u32 index, RHIResource* resource)
	{
		resources[index] = resource;
	}
    
} // namespace CE::RHI
