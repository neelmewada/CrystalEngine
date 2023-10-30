#pragma once

#include "VulkanRHIPrivate.h"

namespace CE
{

    class VulkanPipeline : public RHI::IPipelineState
    {
    public:
        VulkanPipeline(VulkanDevice* device);
        virtual ~VulkanPipeline();

    protected:
        VulkanDevice* device = nullptr;
		VkPipeline pipeline = nullptr;
		VkPipelineLayout pipelineLayout = nullptr;

		List<VkDescriptorSetLayout> setLayouts{};
    };


	class VulkanGraphicsPipeline : public VulkanPipeline, public RHI::GraphicsPipelineState
	{
	public:
		VulkanGraphicsPipeline(VulkanDevice* device, VulkanRenderTarget* renderTarget, const RHI::GraphicsPipelineDesc& desc);
		virtual ~VulkanGraphicsPipeline();

		bool IsGraphicsPipelineState() override final
		{
			return true;
		}

		bool IsComputePipelineState() override final
		{
			return false;
		}

		void* GetNativeHandle() override { return pipeline; }

	protected:

		void Create(VulkanRenderTarget* renderTarget, const RHI::GraphicsPipelineDesc& desc);
		
		void Destroy();

	private:

		Array<RHI::ShaderResourceGroupDesc> resourceGroups{};

	};
    
} // namespace CE::Editor
