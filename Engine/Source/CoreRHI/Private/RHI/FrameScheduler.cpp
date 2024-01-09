#include "CoreRHI.h"

namespace CE::RHI
{
	FrameScheduler::FrameScheduler(const FrameSchedulerDescriptor& descriptor)
	{
		frameGraph = new FrameGraph();
        transientMemoryPool = new TransientMemoryPool();
	}

	FrameScheduler::~FrameScheduler()
	{
        delete transientMemoryPool;
		delete frameGraph;
	}

	void FrameScheduler::BeginFrameGraph()
	{
		FrameGraphBuilder::BeginFrameGraph(frameGraph);
	}

	void FrameScheduler::Construct()
	{
		// Fetch frame graph info
		const Array<RHI::FrameAttachment*>& attachments = frameGraph->attachmentDatabase.GetAttachments();
		
		bufferReq = {};
		u64 bufferOffset = 0;
		imageReq = {};
		u64 imageOffset = 0;

		for (auto attachment : attachments)
		{
			if (attachment->IsBufferAttachment())
			{
				auto bufferAttachment = (RHI::BufferFrameAttachment*)attachment;
				const auto& desc = bufferAttachment->GetBufferDescriptor();
				ResourceMemoryRequirements req{};
				RHI::gDynamicRHI->GetBufferMemoryRequirements(desc, req);
				if (bufferOffset > 0)
					bufferOffset = Memory::GetAlignedSize(bufferOffset, req.offsetAlignment);
				bufferReq.size = bufferOffset + req.size;
				bufferOffset += req.size;
				if (bufferReq.flags == 0)
					bufferReq.flags = req.flags;
				else
					bufferReq.flags &= req.flags;
				
			}
			else if (attachment->IsImageAttachment())
			{
				auto imageAttachment = (RHI::ImageFrameAttachment*)attachment;
				const auto& desc = imageAttachment->GetImageDescriptor();
				ResourceMemoryRequirements req{};
				RHI::gDynamicRHI->GetTextureMemoryRequirements(desc, req);
				if (imageOffset > 0)
					imageOffset = Memory::GetAlignedSize(imageOffset, req.offsetAlignment);
				imageReq.size = imageOffset + req.size;
				imageOffset += req.size;
				if (imageReq.flags == 0)
					imageReq.flags = req.flags;
				else
					imageReq.flags &= req.flags;
			}
		}
	}

    void FrameScheduler::Compile()
    {

    }

    void FrameScheduler::BeginFrame()
	{
		
	}

	void FrameScheduler::EndFrame()
	{

	}

} // namespace CE::RHI
