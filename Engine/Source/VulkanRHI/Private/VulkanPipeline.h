#pragma once

#include "VulkanRHIPrivate.h"

namespace CE
{
	class VulkanPipeline;

	class VulkanPipelineLayout : public RHI::IPipelineLayout
	{
	public:

		VulkanPipelineLayout(VulkanDevice* device, VulkanPipeline* copyFrom);

		VulkanPipelineLayout(VulkanDevice* device, VkPipelineLayout pipelineLayout, RHI::PipelineType pipelineType);
		virtual ~VulkanPipelineLayout();

		virtual RHI::PipelineType GetPipelineType() override
		{
			return pipelineType;
		}

		inline VkPipelineLayout GetNativeHandle() const
		{
			return handle;
		}

		void CopyFrom(VulkanDevice* device, VulkanPipeline* copyFrom);

	private:
		VulkanDevice* device = nullptr;
		VkPipelineLayout handle = nullptr;
		
		RHI::PipelineType pipelineType = RHI::PipelineType::Graphics;

		List<VkDescriptorSetLayout> setLayouts{};
		List<VkPushConstantRange> pushConstantRanges{};
		HashMap<int, Array<VkDescriptorSetLayoutBinding>> setLayoutBindingsMap{};

		friend class VulkanGraphicsPipeline;
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
		List<VkPushConstantRange> pushConstantRanges{};
		HashMap<int, Array<VkDescriptorSetLayoutBinding>> setLayoutBindingsMap{};

		friend class VulkanGraphicsCommandList;
		friend class VulkanPipelineLayout;
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
		friend class VulkanPipelineLayout;
	};
    
} // namespace CE::Editor
