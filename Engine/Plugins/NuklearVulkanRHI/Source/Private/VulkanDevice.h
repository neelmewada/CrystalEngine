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

        void Initialize();
        void PreShutdown();
        void Shutdown();

        // - Public API -

        VkFormat FindSupportedFormat(const Array<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        VkSurfaceFormatKHR FindAutoColorFormat();

        bool CheckSurfaceFormatSupport(VkFormat format);

        s32 FindMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties);

        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageViewType imageViewType, VkImageAspectFlags aspectFlags);

        // - Getters -

        CE_INLINE VulkanQueue* GetGraphicsQueue() { return graphicsQueue; }
        CE_INLINE VulkanQueue* GetPresentQueue() { return presentQueue; }

        CE_INLINE VkDevice GetHandle() { return device; }
        CE_INLINE VkPhysicalDevice GetPhysicalHandle() { return gpu; }

        CE_INLINE const SurfaceSupportInfo& GetSurfaceSupportInfo() const { return surfaceSupport; }

    private:

        void SelectGpu();
        void InitGpu();
        bool QueryGpu(u32 gpuIndex);

        VulkanQueueFamilies GetQueueFamilies(VkPhysicalDevice gpu);
        VkDeviceSize GetPhysicalDeviceLocalMemory(VkPhysicalDevice gpu);
        SurfaceSupportInfo FetchSurfaceSupportInfo(VkPhysicalDevice gpu);

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
        VulkanQueue* graphicsQueue = nullptr;
        VulkanQueue* presentQueue = nullptr;
        VkCommandPool gfxCommandPool = nullptr;

        HashMap<u32, VkCommandPool> queueFamilyToCmdPool{};
    };
    
} // namespace CE
