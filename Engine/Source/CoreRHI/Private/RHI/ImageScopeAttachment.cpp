#include "CoreRHI.h"

namespace CE::RHI
{

	ImageScopeAttachment::ImageScopeAttachment(Scope* scope,
		FrameAttachment* attachment,
		ScopeAttachmentUsage usage,
		ScopeAttachmentAccess access,
		const ImageScopeAttachmentDescriptor& descriptor) : ScopeAttachment(scope, attachment, usage, access, descriptor)
	{

	}

	ImageScopeAttachment::~ImageScopeAttachment()
    {

    }

} // namespace CE::RHI
