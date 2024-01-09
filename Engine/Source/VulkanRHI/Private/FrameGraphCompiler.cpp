#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    
	FrameGraphCompiler::FrameGraphCompiler(VulkanDevice* device) : device(device)
	{

	}

	FrameGraphCompiler::~FrameGraphCompiler()
	{

	}

	void FrameGraphCompiler::CompileCrossQueueScopes(const FrameGraphCompilerRequest& compileRequest)
	{
		FrameGraph* frameGraph = compileRequest.frameGraph;
		
		// TODO: Allocate proper queues
	}

	void FrameGraphCompiler::CompileInternal(const FrameGraphCompilerRequest& compileRequest)
	{

	}

} // namespace CE::Vulkan
