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

	void FrameAttachment::SetResource(int index, RHIResource* resource)
	{
		resources[index] = resource;
	}

    RHIResource* FrameAttachment::GetResource(int imageIndex)
    {
        return resources[imageIndex];
    }

	RHIResource* FrameAttachment::GetResource()
	{
		return resources[0];
	}
    
} // namespace CE::RHI
