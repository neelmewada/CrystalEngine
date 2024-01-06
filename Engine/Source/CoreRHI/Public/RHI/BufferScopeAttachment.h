#pragma once

namespace CE::RHI
{
	struct BufferScopeAttachmentDescriptor : ScopeAttachmentDescriptor
	{

	};
    
	class CORERHI_API BufferScopeAttachment : public ScopeAttachment
	{
	public:

		using DescriptorType = BufferScopeAttachmentDescriptor;

		BufferScopeAttachment(Scope* scope,
			FrameAttachment* attachment,
			ScopeAttachmentUsage usage,
			ScopeAttachmentAccess access,
			const BufferScopeAttachmentDescriptor& descriptor);

		virtual ~BufferScopeAttachment();

		virtual bool IsBufferAttachment() const override final { return true; }
		virtual bool IsImageAttachment() const override final { return false; }

	protected:

	};

} // namespace CE::RHI
