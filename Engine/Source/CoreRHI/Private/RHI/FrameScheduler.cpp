#include "CoreRHI.h"

namespace CE::RHI
{
	FrameScheduler::FrameScheduler(const FrameSchedulerDescriptor& descriptor)
	{
		frameGraph = new FrameGraph();
	}

	FrameScheduler::~FrameScheduler()
	{
		delete frameGraph;
	}

	void FrameScheduler::BeginFrame()
	{
		
	}

	void FrameScheduler::EndFrame()
	{

	}

} // namespace CE::RHI
