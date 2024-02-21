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

        inline bool operator==(const PipelineRenderPass& other) const
        {
            return GetHash() == other.GetHash();
        }
    };
    
    class GraphicsPipeline : public Pipeline
    {
    public:

        GraphicsPipeline(VulkanDevice* device, const RHI::GraphicsPipelineDescriptor& desc);
        virtual ~GraphicsPipeline();

        virtual bool IsGraphicsPipeline() const override final { return true; }

        VkPipeline FindOrCompile(RenderPass* renderPass, u32 subpass, u32 numViewports = 1, u32 numScissors = 1);

        inline void Compile(const Array<PipelineRenderPass>& passes)
        {
            for (const auto& pass : passes)
            {
                Compile(pass.pass, pass.subpass);
            }
        }

        inline void Compile(RenderPass* renderPass, u32 subpass)
        {
            FindOrCompile(renderPass, subpass);
        }

        virtual VkPipelineBindPoint GetBindPoint() override { return VK_PIPELINE_BIND_POINT_GRAPHICS; }

    private:

        VkPipeline CompileInternal(RenderPass* renderPass, u32 subpass, u32 numViewports = 1, u32 numScissors = 1);

        void SetupColorBlendState();
        void SetupDepthStencilState();
        void SetupShaderStages();
        void SetupRasterState();
        void SetupMultisampleState();
        void SetupDefaultRenderPass();

        void SetupVertexInputState();

        RenderPass* defaultRenderPass = nullptr;

        HashMap<SIZE_T, VkPipeline> pipelinesByHash{};
        HashMap<SIZE_T, VkPipelineCache> pipelineCachesByHash{};
        HashMap<PipelineRenderPass, VkPipeline> pipelines{};
        HashMap<PipelineRenderPass, VkPipelineCache> pipelineCaches{};

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
