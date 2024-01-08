#pragma once

namespace CE::RHI
{

	class CORERHI_API BufferFrameAttachment : public FrameAttachment
	{
	public:
		//! Create a transient image attachment
		BufferFrameAttachment(AttachmentID id, const BufferDescriptor& descriptor);

		//! Create an external image attachment 
		BufferFrameAttachment(AttachmentID id, Buffer* buffer);

		virtual ~BufferFrameAttachment();

		virtual bool IsBufferAttachment() const override final { return true; }
		virtual bool IsImageAttachment() const override final { return false; }

	protected:
		BufferDescriptor descriptor{};
	};
    
} // namespace CE::RHI
