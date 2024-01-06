#include "CoreRHI.h"

namespace CE::RHI
{
    BufferFrameAttachment::BufferFrameAttachment(AttachmentID id, const BufferDescriptor& descriptor)
		: FrameAttachment(id, AttachmentLifetimeType::Transient), descriptor(descriptor)
    {

    }

    BufferFrameAttachment::BufferFrameAttachment(AttachmentID id, Buffer* buffer)
		: FrameAttachment(id, AttachmentLifetimeType::External)
    {
		if (buffer)
			SetResource(buffer);
    }

    BufferFrameAttachment::~BufferFrameAttachment()
    {

    }

} // namespace CE::RHI
