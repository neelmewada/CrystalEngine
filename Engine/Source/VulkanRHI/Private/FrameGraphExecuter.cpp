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
		return false;
	}
    
} // namespace CE::Vulkan
