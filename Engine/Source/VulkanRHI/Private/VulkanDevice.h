#pragma once

#include "CoreTypes.h"

#include "VulkanRHI.h"
#include "VulkanStructs.h"
#include "VulkanQueue.h"

#include <vulkan/vulkan.h>

namespace CE::Vulkan
{
    class VulkanQueue;
    class VulkanSwapChain;
    class Texture;
	class VulkanDescriptorPool;
	class ShaderResourceManager;

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

		INLINE bool IsUnifiedMemoryArchitecture() const
		{
			return isUnifiedMemory;
		}

		INLINE bool IsOffscreenMode() const { return !surfaceSupported; }

        INLINE VkCommandPool GetGraphicsCommandPool() const { return gfxCommandPool; }

        INLINE VulkanQueue* GetGraphicsQueue() const { return graphicsQueue; }
        INLINE VulkanQueue* GetPresentQueue() const { return presentQueue; }

        INLINE VkDevice GetHandle() const { return device; }
        INLINE VkPhysicalDevice GetPhysicalHandle() const { return gpu; }

        INLINE const SurfaceSupportInfo& GetSurfaceSupportInfo() const { return surfaceSupport; }

		INLINE bool IsDeviceExtensionSupported(const String& name) const
		{
			return supportedDeviceExtensions.Exists(name);
		}

		INLINE VulkanDescriptorPool* GetDescriptorPool() const { return descriptorPool; }

		INLINE const VkPhysicalDeviceLimits& GetDeviceLimits() const
		{
			return gpuProperties.limits;
		}

		INLINE ShaderResourceManager* GetShaderResourceManager() const
		{
			return srgManager;
		}

		INLINE const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() const
		{
			return memoryProperties;
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
		bool isUnifiedMemory = false;

		/// @brief Vulkan can be initialized without any Native windows (i.e. offscreen mode).
		/// Potentially useful for command line applications/toos.
		bool surfaceSupported = false;

        VkInstance instance = nullptr;
        VulkanRHI* vulkanRhi = nullptr;

		VkPhysicalDeviceMemoryProperties memoryProperties{};
		HashSet<int> ignoredHeapIndices{};

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
		ShaderResourceManager* srgManager = nullptr;
        VkCommandPool gfxCommandPool = nullptr;

        HashMap<u32, VkCommandPool> queueFamilyToCmdPool{};

		SharedMutex mainThreadMutex{};

		VulkanDescriptorPool* descriptorPool = nullptr;
    };
    
} // namespace CE
