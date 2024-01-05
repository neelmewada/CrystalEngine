#include "CoreRHI.h"

namespace CE::RHI
{

    ScopeAttachment::ScopeAttachment(Scope* scope,
                                     FrameAttachment* attachment, 
                                     ScopeAttachmentUsage usage,
                                     ScopeAttachmentAccess access)
        : scope(scope), attachment(attachment), usage(usage), access(access)
    {
        
    }
    
    ScopeAttachment::~ScopeAttachment()
    {
        
    }

} // namespace CE::RHI
