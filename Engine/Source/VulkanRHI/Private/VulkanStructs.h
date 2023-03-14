#pragma once

#include "CoreTypes.h"

#include "vulkan/vulkan.h"

namespace CE
{
    struct GpuMetaData
    {
        u64 localMemorySize = 0;
    };

    struct VulkanQueueFamilies
    {
        s32 graphicsFamily = -1;
        s32 presentFamily = -1;
        s32 computeFamily = -1;
        s32 transferFamily = -1;

        bool IsValidGraphicsFamily() { return graphicsFamily >= 0; }
        bool IsValidPresentFamily() { return presentFamily >= 0; }
        bool IsValidComputeFamily() { return computeFamily >= 0; }
        bool IsValidTransferFamily() { return transferFamily >= 0; }
    };

    struct SurfaceSupportInfo
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;   // TestSurface properties: image size, extent, etc
        Array<VkSurfaceFormatKHR> surfaceFormats;  // TestSurface image supported formats
        Array<VkPresentModeKHR> presentationModes; // How images should be presented to the screen
    };
    
} // namespace CE
