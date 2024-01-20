#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    GraphicsPipeline::GraphicsPipeline(VulkanDevice* device, const RHI::GraphicsPipelineDescriptor& desc)
        : Pipeline(device)
        , desc(desc)
    {
        
    }

    GraphicsPipeline::~GraphicsPipeline()
    {

    }

} // namespace CE::Vulkan
