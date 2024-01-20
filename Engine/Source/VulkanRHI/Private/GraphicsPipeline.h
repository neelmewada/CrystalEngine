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

        RHI::GraphicsPipelineDescriptor desc{};
    };

} // namespace CE::Vulkan
