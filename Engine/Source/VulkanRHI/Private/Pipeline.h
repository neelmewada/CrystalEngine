#pragma once

namespace CE::Vulkan
{
	class PipelineLayout : public RHI::IPipelineLayout
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

		//! Returns true IF ALL of the bindings of setLayoutBindingsMap of 'this' pipeline are present in 'other'
		bool IsCompatibleWith(PipelineLayout* other);

		virtual bool IsCompatibleWith(IPipelineLayout* other) override
		{
			if (other == nullptr)
				return false;
			return IsCompatibleWith(static_cast<PipelineLayout*>(other));
		}

	protected:
		VulkanDevice* device = nullptr;
		VkPipelineLayout pipelineLayout = nullptr;
		VkDescriptorSetLayout emptySetLayout = nullptr;

		RHI::PipelineStateType pipelineType = RHI::PipelineStateType::Graphics;

		List<VkDescriptorSetLayout> setLayouts{};
		List<VkPushConstantRange> pushConstantRanges{};
		HashMap<int, List<VkDescriptorSetLayoutBinding>> setLayoutBindingsMap{};
		HashMap<Name, VkDescriptorSetLayoutBinding> setLayoutBindingsByName{};

		HashMap<RHI::SRGType, ShaderResourceGroupLayout> srgLayouts{};

		friend class GraphicsPipeline;
		friend class GraphicsCommandList;
		friend class PipelineState;
		friend class Scope;
		friend class FrameGraphCompiler;
	};

    class Pipeline : public PipelineLayout, public RHI::RHIResource
    {
    public:
        Pipeline(VulkanDevice* device, const PipelineDescriptor& desc);
        ~Pipeline();

        virtual bool IsGraphicsPipeline() const { return false; }

		inline SIZE_T GetHash() const { return hash; }

    protected:

        VkPipeline pipeline = nullptr;
		VkPipelineCache pipelineCache = nullptr;
		
		Name name{};

		SIZE_T hash = 0;
    };
    
} // namespace CE::Vulkan
