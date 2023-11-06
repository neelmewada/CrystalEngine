#pragma once

#include "VulkanRHIPrivate.h"

namespace CE
{
	class VulkanPipelineLayout : public RHI::IPipelineLayout
	{
	public:
		VulkanPipelineLayout(VulkanDevice* device, VkPipelineLayout pipelineLayout);
		virtual ~VulkanPipelineLayout();

		VulkanDevice* device = nullptr;
		VkPipelineLayout handle = nullptr;
	};

    class VulkanPipeline : public RHI::IPipelineState
    {
    public:
        VulkanPipeline(VulkanDevice* device);
        virtual ~VulkanPipeline();

    protected:
        VulkanDevice* device = nullptr;
		VkPipeline pipeline = nullptr;
		VulkanPipelineLayout* pipelineLayout = nullptr;

		List<VkDescriptorSetLayout> setLayouts{};

		friend class VulkanGraphicsCommandList;
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

		RHI::IPipelineLayout* GetPipelineLayout() override
		{
			return pipelineLayout;
		}

	protected:

		void Create(VulkanRenderTarget* renderTarget, const RHI::GraphicsPipelineDesc& desc);
		
		void Destroy();

	private:

		friend class VulkanGraphicsCommandList;
	};
    
} // namespace CE::Editor
