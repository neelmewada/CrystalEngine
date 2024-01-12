#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

	FrameGraphExecuter::FrameGraphExecuter(VulkanDevice* device) : device(device)
	{
		
	}

	FrameGraphExecuter::~FrameGraphExecuter()
	{

	}

	bool FrameGraphExecuter::ExecuteInternal(const FrameGraphExecuteRequest& executeRequest)
	{
		FrameGraph* frameGraph = executeRequest.frameGraph;

		const Array<RHI::Scope*>& producers = frameGraph->producers;
		

		return true;
	}
    
} // namespace CE::Vulkan
