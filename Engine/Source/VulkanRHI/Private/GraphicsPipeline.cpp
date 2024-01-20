#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    GraphicsPipeline::GraphicsPipeline(VulkanDevice* device, const RHI::GraphicsPipelineDescriptor& desc)
        : Pipeline(device, desc)
        , desc(desc)
    {
        auto srgManager = device->GetShaderResourceManager();

        VkPipelineLayoutCreateInfo layoutCI{};
        layoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        
        
    }

    GraphicsPipeline::~GraphicsPipeline()
    {

    }

    void GraphicsPipeline::Create(RenderPass* renderPass, int subpass)
    {
        VkGraphicsPipelineCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        

    }

} // namespace CE::Vulkan
