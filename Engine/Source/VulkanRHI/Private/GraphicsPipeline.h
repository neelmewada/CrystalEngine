#pragma once

namespace CE::Vulkan
{


    
    class GraphicsPipeline : public Pipeline
    {
    public:

        GraphicsPipeline(VulkanDevice* device, const RHI::GraphicsPipelineDescriptor& desc);
        virtual ~GraphicsPipeline();

        virtual bool IsGraphicsPipeline() const override final { return true; }

    private:

        void Create(RenderPass* renderPass, int subpass);

        HashMap<PipelineRenderPass, VkPipeline> pipelines{};

        RHI::GraphicsPipelineDescriptor desc{};
    };

} // namespace CE::Vulkan
