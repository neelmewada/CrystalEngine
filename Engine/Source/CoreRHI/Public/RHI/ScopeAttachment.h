#pragma once

namespace CE::RHI
{
	class FrameAttachment;
	class Scope;

	struct ScopeAttachmentDescriptor
	{
		AttachmentID attachmentId{};
		AttachmentLoadStoreAction loadStoreAction{};
		MultisampleState multisampleState{};
	};

	struct RenderAttachmentDescriptor
	{
		AttachmentID attachmentId{};
		AttachmentLoadStoreAction loadStoreAction{};
		ScopeAttachmentUsage usage{};
		ScopeAttachmentAccess access{};
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

		inline const AttachmentID& GetId() const { return id; }

		inline FrameAttachment* GetFrameAttachment() const { return attachment; }

		inline ScopeAttachmentUsage GetUsage() const { return usage; }

		inline ScopeAttachmentAccess GetAccess() const { return access; }

		inline const AttachmentLoadStoreAction& GetLoadStoreAction() const { return loadStoreAction; }

		inline Scope* GetOwner() const { return scope; }

		inline const MultisampleState& GetMultisampleState() const { return multisampleState; }

	protected:

		AttachmentID id{};

		Scope* scope = nullptr;

		FrameAttachment* attachment = nullptr;

		ScopeAttachmentUsage usage{};

		ScopeAttachmentAccess access{};

		AttachmentLoadStoreAction loadStoreAction{};

		MultisampleState multisampleState{};

		friend class Scope;
		friend class FrameGraph;
		friend class FrameGraphBuilder;
	};

} // namespace CE::RHI
