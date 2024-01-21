#pragma once

namespace CE::Vulkan
{
	class PipelineLayout
	{
	public:
		virtual ~PipelineLayout();

		inline RHI::PipelineStateType GetPipelineType() const
		{
			return pipelineType;
		}

		inline VkPipelineLayout GetVkPipelineLayout() const
		{
			return pipelineLayout;
		}

	protected:
		VulkanDevice* device = nullptr;
		VkPipelineLayout pipelineLayout = nullptr;

		RHI::PipelineStateType pipelineType = RHI::PipelineStateType::Graphics;

		List<VkDescriptorSetLayout> setLayouts{};
		List<VkPushConstantRange> pushConstantRanges{};
		HashMap<int, List<VkDescriptorSetLayoutBinding>> setLayoutBindingsMap{};

		friend class GraphicsPipeline;
		friend class GraphicsCommandList;
		friend class PipelineState;
	};

    class Pipeline : public PipelineLayout, public RHI::RHIResource
    {
    public:
        Pipeline(VulkanDevice* device, const PipelineDescriptor& desc);
        ~Pipeline();

        virtual bool IsGraphicsPipeline() const { return false; }

    protected:

        VkPipeline pipeline = nullptr;
		

    };
    
} // namespace CE::Vulkan
