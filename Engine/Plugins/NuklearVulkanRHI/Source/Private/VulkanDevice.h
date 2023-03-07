#pragma once

#include "CoreTypes.h"

#include "NuklearVulkanRHI.h"
#include "VulkanStructs.h"
#include "VulkanQueue.h"

#include <vulkan/vulkan.h>

namespace CE
{
    class VulkanQueue;
    class VulkanSwapChain;

    class VulkanDevice
    {
        CE_NO_COPY(VulkanDevice)
    public:
        VulkanDevice(VkInstance instance, NuklearVulkanRHI* vulkanRhi);
        ~VulkanDevice();

        CE_INLINE bool IsInitialized() const
        {
            return isInitialized;
        }

        CE_INLINE VkDevice GetHandle() const
        {
            return device;
        }

        void Initialize();
        void PreShutdown();
        void Shutdown();

        // - Public API -

        VkFormat FindSupportedFormat(const Array<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        VkSurfaceFormatKHR FindAutoColorFormat();

        bool CheckSurfaceFormatSupport(VkFormat format);

    private:

        void SelectGpu();
        void InitGpu();
        bool QueryGpu(u32 gpuIndex);

        VulkanQueueFamilies GetQueueFamilies(VkPhysicalDevice gpu);
        VkDeviceSize GetPhysicalDeviceLocalMemory(VkPhysicalDevice gpu);
        SurfaceSupportInfo GetSurfaceSupportInfo(VkPhysicalDevice gpu);

        bool isInitialized = false;
        VkInstance instance = nullptr;
        VmaAllocator vmaAllocator = nullptr;
        NuklearVulkanRHI* vulkanRhi = nullptr;

        VkPhysicalDevice gpu = nullptr;
        VkPhysicalDeviceProperties gpuProperties{};
        GpuMetaData gpuMetaData{};

        VkDevice device = nullptr;
        VulkanQueueFamilies queueFamilies{};
        SurfaceSupportInfo surfaceSupport{};

        VkSurfaceKHR testSurface = nullptr;
        VulkanQueue* graphicsQueue;
        VulkanQueue* presentQueue;
        VkCommandPool gfxCommandPool = nullptr;

        HashMap<u32, VkCommandPool> queueFamilyToCmdPool{};
    };
    
} // namespace CE
