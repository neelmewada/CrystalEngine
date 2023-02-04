#pragma once

#include "NuklearVulkanRHI.h"
#include "VulkanStructs.h"

#include "vulkan/vulkan.h"

namespace CE
{

    class VulkanDevice
    {
        CE_NO_COPY(VulkanDevice)
    public:
        VulkanDevice(VkInstance instance, NuklearVulkanRHI* vulkanRhi);
        ~VulkanDevice();

        void Initialize();
        void PreShutdown();
        void Shutdown();

    private:

        void SelectGpu();
        void InitGpu();
        bool QueryGpu(u32 gpuIndex);

        VulkanQueueFamilies GetQueueFamilies(VkPhysicalDevice gpu);
        VkDeviceSize GetPhysicalDeviceLocalMemory(VkPhysicalDevice gpu);
        SurfaceSupportInfo GetSurfaceSupportInfo(VkPhysicalDevice gpu);

        VkInstance instance = nullptr;
        NuklearVulkanRHI* vulkanRhi = nullptr;

        VkPhysicalDevice gpu = nullptr;
        VkPhysicalDeviceProperties gpuProperties{};
        GpuMetaData gpuMetaData{};

        VkDevice device = nullptr;
        VulkanQueueFamilies queueFamilies{};
        SurfaceSupportInfo surfaceSupport{};

        VkSurfaceKHR testSurface = nullptr;
    };
    
} // namespace CE
