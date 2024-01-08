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

	void FrameScheduler::BeginFrame()
	{
		
	}

	void FrameScheduler::EndFrame()
	{

	}

} // namespace CE::RHI
