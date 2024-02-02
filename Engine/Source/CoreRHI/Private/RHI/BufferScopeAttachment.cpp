#include "CoreRHI.h"

namespace CE::RHI
{
    BufferScopeAttachment::BufferScopeAttachment(Scope* scope, 
		FrameAttachment* attachment,
		ScopeAttachmentUsage usage, 
		ScopeAttachmentAccess access,
		const BufferScopeAttachmentDescriptor& descriptor)
		: ScopeAttachment(scope, attachment, usage, access, descriptor)
    {

    }

    BufferScopeAttachment::~BufferScopeAttachment()
    {

    }

} // namespace CE::RHI
