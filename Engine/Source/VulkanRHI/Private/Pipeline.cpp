#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    PipelineLayout::~PipelineLayout()
    {
        if (pipelineLayout)
        {
            vkDestroyPipelineLayout(device->GetHandle(), pipelineLayout, nullptr);
            pipelineLayout = nullptr;
        }
    }

    Pipeline::Pipeline(VulkanDevice* device)
        : RHI::RHIResource(RHI::ResourceType::Pipeline)
    {
        this->device = device;
    }

    Pipeline::~Pipeline()
    {

    }

} // namespace CE::Vulkan
