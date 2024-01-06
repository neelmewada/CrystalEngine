#include "CoreRHI.h"

namespace CE::RHI
{
    FrameAttachmentDatabase::FrameAttachmentDatabase()
    {
        
    }
    
    FrameAttachmentDatabase::~FrameAttachmentDatabase()
    {
		for (auto attachment : attachments)
		{
			delete attachment;
		}
		attachments.Clear();
		attachmentsById.Clear();
    }

	void FrameAttachmentDatabase::EmplaceFrameAttachment(AttachmentID id, const ImageDescriptor& descriptor)
	{
		auto attachment = new ImageFrameAttachment(id, descriptor);
		attachments.Add(attachment);
		attachmentsById[id] = attachment;
	}

	void FrameAttachmentDatabase::EmplaceFrameAttachment(AttachmentID id, Texture* image)
	{
		auto attachment = new ImageFrameAttachment(id, image);
		attachments.Add(attachment);
		attachmentsById[id] = attachment;
	}

	void FrameAttachmentDatabase::EmplaceFrameAttachment(AttachmentID id, const BufferDescriptor& descriptor)
	{
		auto attachment = new BufferFrameAttachment(id, descriptor);
	}

	FrameAttachment* FrameAttachmentDatabase::FindFrameAttachment(AttachmentID id)
	{
		if (attachmentsById.KeyExists(id))
			return attachmentsById[id];
		return nullptr;
	}

} // namespace CE::RHI
