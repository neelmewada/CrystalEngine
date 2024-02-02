#include "CoreRHI.h"

namespace CE::RHI
{

	ScopeAttachment::ScopeAttachment(Scope* scope,
		FrameAttachment* attachment,
		ScopeAttachmentUsage usage,
		ScopeAttachmentAccess access,
		const ScopeAttachmentDescriptor& descriptor)
		: scope(scope)
		, id(descriptor.attachmentId)
		, attachment(attachment)
		, usage(usage), access(access)
		, loadStoreAction(descriptor.loadStoreAction)
		, multisampleState(descriptor.multisampleState)
	{
	}

	ScopeAttachment::~ScopeAttachment()
    {
        
    }

} // namespace CE::RHI
