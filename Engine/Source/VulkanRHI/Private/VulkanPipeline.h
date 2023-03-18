#pragma once

#include "VulkanRHIPrivate.h"

namespace CE::Editor
{

    class VulkanPipeline
    {
    public:
        VulkanPipeline(VulkanDevice* device);
        virtual ~VulkanPipeline();

    protected:
        VulkanDevice* device = nullptr;
    };
    
} // namespace CE::Editor
