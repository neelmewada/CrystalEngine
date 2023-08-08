#pragma once

#include "CoreTypes.h"

#include "VulkanRHI.h"
#include "VulkanStructs.h"
#include "VulkanQueue.h"

#include <vulkan/vulkan.h>

namespace CE
{
    class VulkanQueue;
    class VulkanSwapChain;
    class VulkanTexture;

    class VulkanDevice
    {
        CE_NO_COPY(VulkanDevice)
    public:
        VulkanDevice(VkInstance instance, VulkanRHI* vulkanRhi);
        ~VulkanDevice();

        INLINE bool IsInitialized() const
        {
            return isInitialized;
        }

        void Initialize();
        void PreShutdown();
        void Shutdown();

        // - Public API -

		void WaitUntilIdle();

        VkFormat FindSupportedFormat(const Array<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        VkSurfaceFormatKHR FindAutoColorFormat();

        bool CheckSurfaceFormatSupport(VkFormat format);

        s32 FindMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties);

        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageViewType imageViewType, VkImageAspectFlags aspectFlags);

        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT);

        VkCommandBuffer BeginSingleUseCommandBuffer();
        void EndSingleUseCommandBuffer(VkCommandBuffer commandBuffer);
        void SubmitAndWaitSingleUseCommandBuffer(VkCommandBuffer commandBuffer);

        // - Getters -

        INLINE VkCommandPool GetGraphicsCommandPool() { return gfxCommandPool; }

        INLINE VulkanQueue* GetGraphicsQueue() { return graphicsQueue; }
        INLINE VulkanQueue* GetPresentQueue() { return presentQueue; }

        INLINE VkDevice GetHandle() { return device; }
        INLINE VkPhysicalDevice GetPhysicalHandle() { return gpu; }

        INLINE const SurfaceSupportInfo& GetSurfaceSupportInfo() const { return surfaceSupport; }

		INLINE bool IsDeviceExtensionSupported(const String& name) const
		{
			return supportedDeviceExtensions.Exists(name);
		}

    protected:

    private:

        void SelectGpu();
        void InitGpu();
        bool QueryGpu(u32 gpuIndex);

        VulkanQueueFamilies GetQueueFamilies(VkPhysicalDevice gpu);
        VkDeviceSize GetPhysicalDeviceLocalMemory(VkPhysicalDevice gpu);
        SurfaceSupportInfo FetchSurfaceSupportInfo(VkPhysicalDevice gpu);

        bool isInitialized = false;
        VkInstance instance = nullptr;
        VulkanRHI* vulkanRhi = nullptr;

		Array<String> supportedDeviceExtensions{};

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
