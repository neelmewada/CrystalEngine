#pragma once

namespace CE::Vulkan
{

    struct PipelineRenderPass
    {
        RenderPass* pass = nullptr;
        u32 subpass = 0;

        inline SIZE_T GetHash() const
        {
            SIZE_T hash = pass->GetHash();
            CombineHash(hash, subpass);
            return hash;
        }
    };
    
    class GraphicsPipeline : public Pipeline
    {
    public:

        GraphicsPipeline(VulkanDevice* device, const RHI::GraphicsPipelineDescriptor& desc);
        virtual ~GraphicsPipeline();

        virtual bool IsGraphicsPipeline() const override final { return true; }

    private:

        void Create(RenderPass* renderPass, int subpass);

        void SetupColorBlendState();
        void SetupDepthStencilState();
        void SetupShaderStages();
        void SetupRasterState();

        void SetupVertexInputState();

        HashMap<PipelineRenderPass, VkPipeline> pipelines{};

        RHI::GraphicsPipelineDescriptor desc{};

        VkPipelineColorBlendStateCreateInfo colorBlendState{};
        List<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};

        VkPipelineDepthStencilStateCreateInfo depthStencilState{};
        VkPipelineRasterizationStateCreateInfo rasterState{};

        List<VkPipelineShaderStageCreateInfo> shaderStages{};

        List<VkVertexInputAttributeDescription> vertexInputDescriptions{};
        List<VkVertexInputBindingDescription> vertexBindingDescriptions{};

        VkPipelineMultisampleStateCreateInfo multisampleState{};
    };

} // namespace CE::Vulkan
