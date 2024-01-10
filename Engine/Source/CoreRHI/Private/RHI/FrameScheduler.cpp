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

    FrameAttachment* FrameScheduler::GetFrameAttachment(AttachmentID id)
    {
        return GetAttachmentDatabase().FindFrameAttachment(id);
    }

} // namespace CE::RHI
