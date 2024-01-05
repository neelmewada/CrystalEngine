#pragma once

namespace CE::RHI
{
	class FrameAttachment;
	class Scope;

	class CORERHI_API ScopeAttachment
	{
	public:


	protected:

		Scope* scope = nullptr;

		FrameAttachment* attachment = nullptr;

		ScopeAttachmentAccess usage{};

		ScopeAttachmentAccess access{};

		AttachmentLoadStoreAction loadStoreAction{};

	};

} // namespace CE::RHI
