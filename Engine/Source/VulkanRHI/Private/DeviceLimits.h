#pragma once

namespace CE::Vulkan
{
    class VulkanDevice;

    class DeviceLimits : public RHI::DeviceLimits
    {
    public:
        DeviceLimits(VulkanDevice* device);


    private:

    };
    
} // namespace CE::Vulkan
