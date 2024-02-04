#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

    PipelineState::PipelineState(VulkanDevice* device, const RHI::GraphicsPipelineDescriptor& graphicsDesc) : device(device)
    {
		this->graphicsDescriptor = graphicsDesc;
		pipeline = new GraphicsPipeline(device, graphicsDesc);
    }

    PipelineState::~PipelineState()
    {
		delete pipeline;
    }

	IPipelineLayout* PipelineState::GetPipelineLayout()
	{
		return pipeline;
	}

} // namespace CE
