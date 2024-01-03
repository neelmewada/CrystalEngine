#pragma once

#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
	class PipelineState;
	class GraphicsPipelineState;

	class PipelineLayout : public RHI::IPipelineLayout
	{
	public:

		PipelineLayout(VulkanDevice* device, PipelineState* copyFrom);

		PipelineLayout(VulkanDevice* device, VkPipelineLayout pipelineLayout, RHI::PipelineType pipelineType);
		virtual ~PipelineLayout();

		virtual RHI::PipelineType GetPipelineType() override
		{
			return pipelineType;
		}

		inline VkPipelineLayout GetNativeHandle() const
		{
			return handle;
		}

		void CopyFrom(VulkanDevice* device, PipelineState* copyFrom);

	private:
		VulkanDevice* device = nullptr;
		VkPipelineLayout handle = nullptr;
		
		RHI::PipelineType pipelineType = RHI::PipelineType::Graphics;

		List<VkDescriptorSetLayout> setLayouts{};
		List<VkPushConstantRange> pushConstantRanges{};
		HashMap<int, Array<VkDescriptorSetLayoutBinding>> setLayoutBindingsMap{};

		friend class GraphicsPipelineState;
		friend class GraphicsCommandList;
		friend class PipelineState;
	};

    class PipelineState : public RHI::IPipelineState
    {
    public:
        PipelineState(VulkanDevice* device);
        virtual ~PipelineState();

    protected:
        VulkanDevice* device = nullptr;
		VkPipeline pipeline = nullptr;
		PipelineLayout* pipelineLayout = nullptr;

		List<VkDescriptorSetLayout> setLayouts{};
		List<VkPushConstantRange> pushConstantRanges{};
		HashMap<int, Array<VkDescriptorSetLayoutBinding>> setLayoutBindingsMap{};

		friend class GraphicsCommandList;
		friend class PipelineLayout;
		friend class GraphicsPipelineState;
    };


	class GraphicsPipelineState : public PipelineState, public RHI::GraphicsPipelineState
	{
	public:
		GraphicsPipelineState(VulkanDevice* device, RenderTarget* renderTarget, const RHI::GraphicsPipelineDescriptor& desc);
		virtual ~GraphicsPipelineState();

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

		void Create(RenderTarget* renderTarget, const RHI::GraphicsPipelineDescriptor& desc);
		
		void Destroy();

	private:

		friend class GraphicsCommandList;
		friend class PipelineLayout;
	};
    
} // namespace CE::Editor
