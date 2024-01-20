#pragma once

#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
	class PipelineState;
	class GraphicsPipeline;

    class PipelineState : public RHI::PipelineState
    {
    public:
        PipelineState(VulkanDevice* device, const RHI::GraphicsPipelineDescriptor& graphicsDesc);
        virtual ~PipelineState();

    protected:

        VulkanDevice* device = nullptr;

		Pipeline* pipeline = nullptr;

		friend class GraphicsCommandList;
		friend class PipelineLayout;
		friend class GraphicsPipelineState;
    };
    
} // namespace CE::Editor
