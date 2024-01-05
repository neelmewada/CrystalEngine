#pragma once

namespace CE::RHI
{
	class FrameAttachment;
	class Scope;

	class CORERHI_API ScopeAttachment
	{
	public:
        
        ScopeAttachment(Scope* scope,
                        FrameAttachment* attachment,
                        ScopeAttachmentUsage usage,
                        ScopeAttachmentAccess access);
        
        virtual ~ScopeAttachment();

	protected:

		Scope* scope = nullptr;

		FrameAttachment* attachment = nullptr;

		ScopeAttachmentAccess usage{};

		ScopeAttachmentAccess access{};

		AttachmentLoadStoreAction loadStoreAction{};

	};

} // namespace CE::RHI
