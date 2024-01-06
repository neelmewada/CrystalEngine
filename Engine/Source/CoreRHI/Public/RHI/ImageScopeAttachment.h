#pragma once

namespace CE::RHI
{
	class ImageFrameAttachment;

	struct ImageScopeAttachmentDescriptor : ScopeAttachmentDescriptor
	{

	};
    
	class CORERHI_API ImageScopeAttachment : public ScopeAttachment
	{
	public:

		using DescriptorType = ImageScopeAttachmentDescriptor;

		ImageScopeAttachment(Scope* scope,
			FrameAttachment* attachment,
			ScopeAttachmentUsage usage,
			ScopeAttachmentAccess access,
			const ImageScopeAttachmentDescriptor& descriptor);

		virtual ~ImageScopeAttachment();

		virtual bool IsBufferAttachment() const override final { return false; }
		virtual bool IsImageAttachment() const override final { return true; }

	protected:


	};

} // namespace CE::RHI
