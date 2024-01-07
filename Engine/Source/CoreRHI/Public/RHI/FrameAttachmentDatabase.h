#pragma once

namespace CE::RHI
{
    
	class CORERHI_API FrameAttachmentDatabase final
	{
	public:
		FrameAttachmentDatabase();
		virtual ~FrameAttachmentDatabase();

		//! Add a transient image attachment
		void EmplaceFrameAttachment(AttachmentID id, const ImageDescriptor& descriptor);

		//! Add an external image attachment
		void EmplaceFrameAttachment(AttachmentID id, Texture* image);

		//! Add a transient buffer attachment
		void EmplaceFrameAttachment(AttachmentID id, const BufferDescriptor& descriptor);

		//! Add a external buffer attachment
		void EmplaceFrameAttachment(AttachmentID id, Buffer* buffer);

		FrameAttachment* FindFrameAttachment(AttachmentID id);

	private:

		Array<FrameAttachment*> attachments{};
		HashMap<AttachmentID, FrameAttachment*> attachmentsById{};

		friend class FrameGraph;
		friend class FrameGraphBuilder;
	};

} // namespace CE::RHI
