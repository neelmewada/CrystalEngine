#pragma once

namespace CE::RHI
{
    
	class CORERHI_API FrameAttachmentDatabase final
	{
	public:
		FrameAttachmentDatabase();
		virtual ~FrameAttachmentDatabase();


	private:

		Array<FrameAttachment*> attachments{};
		HashMap<AttachmentID, FrameAttachment*> attachmentsById{};

		friend class FrameGraph;
	};

} // namespace CE::RHI
