#pragma once

namespace CE::RHI
{
	class FrameAttachment;
	class Scope;

	struct ScopeAttachmentDescriptor
	{
		AttachmentID attachmentId{};
		AttachmentLoadStoreAction loadStoreAction{};
	};

	class CORERHI_API ScopeAttachment
	{
	public:

		ScopeAttachment(Scope* scope, 
			FrameAttachment* attachment, 
			ScopeAttachmentUsage usage,
			ScopeAttachmentAccess access,
			const ScopeAttachmentDescriptor& descriptor);
        
        virtual ~ScopeAttachment();

		virtual bool IsBufferAttachment() const = 0;
		virtual bool IsImageAttachment() const = 0;

		inline const Name& GetId() const { return id; }

	protected:

		AttachmentID id{};

		Scope* scope = nullptr;

		FrameAttachment* attachment = nullptr;

		ScopeAttachmentUsage usage{};

		ScopeAttachmentAccess access{};

		AttachmentLoadStoreAction loadStoreAction{};

		friend class Scope;
		friend class FrameGraph;
		friend class FrameGraphBuilder;
	};

} // namespace CE::RHI
