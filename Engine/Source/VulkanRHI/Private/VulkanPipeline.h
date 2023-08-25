#pragma once

#include "VulkanRHIPrivate.h"

namespace CE
{

    class VulkanPipeline
    {
    public:
        VulkanPipeline(VulkanDevice* device);
        virtual ~VulkanPipeline();

    protected:
        VulkanDevice* device = nullptr;
		VkPipeline pipeline = nullptr;
    };

	class VulkanGraphicsPipeline : public VulkanPipeline
	{
	public:
		VulkanGraphicsPipeline(VulkanDevice* device, const RHI::GraphicsPipelineDesc& desc);
		virtual ~VulkanGraphicsPipeline();

	protected:

		
	};
    
} // namespace CE::Editor
