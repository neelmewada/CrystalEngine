
#include "VulkanDevice.h"
#include "PAL/Common/VulkanPlatform.h"

#include "VulkanDescriptorPool.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace CE
{

	VulkanDevice::VulkanDevice(VkInstance instance, VulkanRHI* vulkanRhi)
		: instance(instance), vulkanRhi(vulkanRhi)
	{
		
	}

	VulkanDevice::~VulkanDevice()
	{

	}

	void VulkanDevice::Initialize()
	{
		// TODO: pass window handle
		auto mainWindow = VulkanPlatform::GetMainPlatformWindow();
		if (mainWindow != nullptr)
			testSurface = VulkanPlatform::CreateSurface(instance, mainWindow);

		SelectGpu();
		InitGpu();

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilies.graphicsFamily;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		if (vkCreateCommandPool(device, &poolInfo, nullptr, &gfxCommandPool) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Graphics Command Pool");
			return;
		}

		if (testSurface != nullptr)
			VulkanPlatform::DestroySurface(instance, testSurface);
		testSurface = nullptr;

		descriptorPool = new VulkanDescriptorPool(this);

		isInitialized = true;

		CE_LOG(Info, All, "Vulkan device initialized");
	}

	void VulkanDevice::PreShutdown()
	{
		isInitialized = false;

		delete descriptorPool;
		descriptorPool = nullptr;

		delete graphicsQueue;
		graphicsQueue = nullptr;

		delete presentQueue;
		presentQueue = nullptr;
	}

	void VulkanDevice::Shutdown()
	{
		vkDestroyCommandPool(device, gfxCommandPool, nullptr);
		gfxCommandPool = nullptr;

		// Command Pools
		for (const auto& pair : queueFamilyToCmdPool)
		{
			vkDestroyCommandPool(device, pair.second, nullptr);
		}
		queueFamilyToCmdPool.Clear();

		vkDestroyDevice(device, nullptr);
		device = nullptr;

		CE_LOG(Info, All, "Vulkan device shutdown");
	}

	void VulkanDevice::SelectGpu()
	{
		// Fetch all available physical devices
		u32 physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
		std::vector<VkPhysicalDevice> physicalDevices{ physicalDeviceCount };
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

		if (physicalDeviceCount == 1)
		{
			this->gpu = physicalDevices[0];
			vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
			gpuMetaData.localMemorySize = GetPhysicalDeviceLocalMemory(gpu);
			CE_LOG(Info, All, "Selected {} as the target GPU.", gpuProperties.deviceName);
			return;
		}

		if (physicalDeviceCount == 0)
		{
			CE_LOG(Error, All, "Failed to select a target GPU. No Vulkan supported GPU found!");
			return;
		}

		VkDeviceSize selectedDeviceMemorySize = 0;
		u32 selectedGpuIndex = 0;

		for (int i = 0; i < physicalDevices.size(); ++i)
		{
			const auto& physicalDevice = physicalDevices[i];

			VkPhysicalDeviceProperties props = {};
			vkGetPhysicalDeviceProperties(physicalDevice, &props);

			VkPhysicalDeviceFeatures features = {};
			vkGetPhysicalDeviceFeatures(physicalDevice, &features);

			auto localMemorySize = GetPhysicalDeviceLocalMemory(physicalDevice);
			if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
				localMemorySize > selectedDeviceMemorySize)
			{
				if (QueryGpu(i))
				{
					selectedGpuIndex = i;
					selectedDeviceMemorySize = localMemorySize;
				}
			}
		}

		this->gpu = physicalDevices[selectedGpuIndex];
		vkGetPhysicalDeviceProperties(this->gpu, &gpuProperties);

		CE_LOG(Info, All, "Selected {} as the target GPU", gpuProperties.deviceName);
	}

	void VulkanDevice::InitGpu()
	{
		vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
		gpuMetaData.localMemorySize = GetPhysicalDeviceLocalMemory(gpu);

		// Store Queue Families & Surface Support Info for later use
		queueFamilies = GetQueueFamilies(gpu);
		surfaceSupport = FetchSurfaceSupportInfo(gpu);

		Array<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<int> queueFamilyIndices = { queueFamilies.graphicsFamily, queueFamilies.presentFamily };
		float queuePriority = 1.0f;

		u32 graphicsQueueIndex = 0;
		u32 presentQueueIndex = queueFamilyIndices.size() - 1;

		for (const auto& queueFamily : queueFamilyIndices)
		{
			VkDeviceQueueCreateInfo queueCI = {};
			queueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCI.queueFamilyIndex = queueFamily;
			queueCI.queueCount = 1;
			queueCI.pQueuePriorities = &queuePriority;

			queueCreateInfos.Add(queueCI);
		}

		VkDeviceCreateInfo deviceCI = {};
		deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCI.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.GetSize());
		deviceCI.pQueueCreateInfos = queueCreateInfos.GetData();

		auto deviceExtensionNames = Array<const char*>();

		uint32_t deviceExtensionCount = 0;
		vkEnumerateDeviceExtensionProperties(gpu, nullptr, &deviceExtensionCount, nullptr);
		auto deviceExtensionProperties = Array<VkExtensionProperties>(deviceExtensionCount);
		vkEnumerateDeviceExtensionProperties(gpu, nullptr, &deviceExtensionCount, deviceExtensionProperties.GetData());

		supportedDeviceExtensions.Clear();

		for (int i = 0; i < deviceExtensionCount; ++i)
		{
			// Required rule by Vulkan Specs.
			if (strcmp(deviceExtensionProperties[i].extensionName, "VK_KHR_portability_subset") == 0)
			{
				deviceExtensionNames.Add(deviceExtensionProperties[i].extensionName);
			}

			supportedDeviceExtensions.Add(deviceExtensionProperties[i].extensionName);
		}

		auto requiredDeviceExtensions = VulkanPlatform::GetRequiredVulkanDeviceExtensions();

		for (const auto& requiredExtension : requiredDeviceExtensions)
		{
			deviceExtensionNames.Add(requiredExtension);
		}

		// Allow descriptor set bindings to change after binding it once
		deviceExtensionNames.Add(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

		deviceCI.enabledExtensionCount = static_cast<uint32_t>(deviceExtensionNames.GetSize());
		deviceCI.ppEnabledExtensionNames = !deviceExtensionNames.IsEmpty() ? deviceExtensionNames.GetData() : nullptr;

		VkPhysicalDeviceFeatures deviceFeaturesToUse{};
		deviceFeaturesToUse.samplerAnisotropy = VK_TRUE;

		deviceCI.pEnabledFeatures = &deviceFeaturesToUse;

		if (vkCreateDevice(gpu, &deviceCI, nullptr, &device) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create vulkan device.");
			return;
		}

		VkQueue gfxQueue = nullptr, presentQueue = nullptr;
		vkGetDeviceQueue(device, queueFamilies.graphicsFamily, graphicsQueueIndex, &gfxQueue);
		vkGetDeviceQueue(device, queueFamilies.presentFamily, presentQueueIndex, &presentQueue);

		this->graphicsQueue = new VulkanQueue(this, queueFamilies.graphicsFamily, graphicsQueueIndex, gfxQueue);
		this->presentQueue = new VulkanQueue(this, queueFamilies.presentFamily, presentQueueIndex, presentQueue);

		//VmaAllocatorCreateInfo allocatorCI{};
		//allocatorCI.instance = instance;
		//allocatorCI.device = device;
		//allocatorCI.physicalDevice = gpu;

		//if (vmaCreateAllocator(&allocatorCI, &vmaAllocator) != VK_SUCCESS)
		//{
		//	CE_LOG(Error, All, "Failed to create Vulkan VmaAllocator!");
		//	return;
		//}

		// Command Pools
		for (const auto& familyIndex : queueFamilyIndices)
		{
			VkCommandPoolCreateInfo commandPoolCI = {};
			commandPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolCI.queueFamilyIndex = familyIndex;

			VkCommandPool commandPool = nullptr;
			if (vkCreateCommandPool(device, &commandPoolCI, nullptr, &commandPool) != VK_SUCCESS)
			{
				CE_LOG(Error, All, "Failed to create Vulkan Command Pool!");
				return;
			}

			if (!queueFamilyToCmdPool.KeyExists(familyIndex))
				queueFamilyToCmdPool.Add({ (u32)familyIndex, nullptr });

			queueFamilyToCmdPool[familyIndex] = commandPool;
		}

		CE_LOG(Info, All, "Vulkan: Created logical device & queues");
	}

	bool VulkanDevice::QueryGpu(u32 gpuIndex)
	{
		u32 physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
		std::vector<VkPhysicalDevice> physicalDevices{ physicalDeviceCount };
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

		VkPhysicalDevice gpu = physicalDevices[gpuIndex];
		auto queueFamilies = GetQueueFamilies(gpu);

		if (!queueFamilies.IsValidGraphicsFamily() || !queueFamilies.IsValidPresentFamily() || !queueFamilies.IsValidComputeFamily() || !queueFamilies.IsValidTransferFamily())
		{
			return false;
		}

		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, nullptr);

		if (extensionCount == 0)
			return false;

		Array<VkExtensionProperties> supportedExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, supportedExtensions.GetData());

		Array<const char*> requiredVulkanDeviceExtensions = VulkanPlatform::GetRequiredVulkanDeviceExtensions();

		// Check for required extensions in the Physical Device's supported extensions.
		for (auto& requiredExtension : requiredVulkanDeviceExtensions)
		{
			bool hasExtension = false;
			for (const auto& SupportedExtension : supportedExtensions)
			{
				if (strcmp(SupportedExtension.extensionName, requiredExtension) == 0)
				{
					hasExtension = true;
					break;
				}
			}
			if (!hasExtension)
			{
				return false;
			}
		}

		auto surfaceSupportInfo = FetchSurfaceSupportInfo(gpu);
		bool surfaceSupportsSwapChain = !surfaceSupportInfo.presentationModes.IsEmpty() && !surfaceSupportInfo.surfaceFormats.IsEmpty();
		if (!surfaceSupportsSwapChain)
			return false;

		return true;
	}

	VulkanQueueFamilies VulkanDevice::GetQueueFamilies(VkPhysicalDevice gpu)
	{
		VulkanQueueFamilies queueFamilies{};

		u32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, nullptr);
		Array<VkQueueFamilyProperties> queueFamilyProps{ queueFamilyCount };
		vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueFamilyProps.GetData());

		for (int i = 0; i < queueFamilyProps.GetSize(); ++i)
		{
			const auto& queueFamilyProperty = queueFamilyProps[i];

			// Always prefer a single Graphics Queue Family that supports all 3: Graphics, Presentation, Compute
			if (testSurface != nullptr && queueFamilyProperty.queueCount > 0 &&
				(queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				VkBool32 presentationSupported = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, testSurface, &presentationSupported);
				if (presentationSupported)
				{
					queueFamilies.graphicsFamily = queueFamilies.transferFamily = queueFamilies.presentFamily = i;
				}
				else if (!queueFamilies.IsValidGraphicsFamily())
				{
					queueFamilies.graphicsFamily = queueFamilies.transferFamily = i;
				}
				if (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT)
				{
					queueFamilies.computeFamily = i;
				}
			}

			// If we couldn't find a Graphics Queue with our preferred features, then pick any one
			if (!queueFamilies.IsValidGraphicsFamily() &&
				queueFamilyProperty.queueCount > 0 &&
				queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				queueFamilies.graphicsFamily = i;
			}

			// Pick any presentation queue if we couldn't find our preferred one
			if (testSurface != nullptr && !queueFamilies.IsValidPresentFamily() && queueFamilyProperty.queueCount > 0)
			{
				VkBool32 PresentationSupported = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, testSurface, &PresentationSupported);
				if (PresentationSupported)
				{
					queueFamilies.presentFamily = i;
				}
			}

			// Pick any transfer family if we couldn't find our preferred one
			if (!queueFamilies.IsValidTransferFamily() &&
				queueFamilyProperty.queueFlags > 0 &&
				queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				queueFamilies.transferFamily = i;
			}

			// Pick any Compute family if we couldn't find our preferred one
			if (!queueFamilies.IsValidComputeFamily() &&
				queueFamilyProperty.queueCount > 0 &&
				queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				queueFamilies.computeFamily = i;
			}
		}

		return queueFamilies;
	}

	VkDeviceSize VulkanDevice::GetPhysicalDeviceLocalMemory(VkPhysicalDevice gpu)
	{
		// Determine the available pDevice local memory.
		VkPhysicalDeviceMemoryProperties memoryProps;
		vkGetPhysicalDeviceMemoryProperties(gpu, &memoryProps);

		auto heapsPtr = memoryProps.memoryHeaps;
		auto heaps = std::vector<VkMemoryHeap>(heapsPtr, heapsPtr + memoryProps.memoryHeapCount);
		for (const auto& heap : heaps)
		{
			if (heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			{
				return heap.size;
			}
		}
		return 0;
	}

	SurfaceSupportInfo VulkanDevice::FetchSurfaceSupportInfo(VkPhysicalDevice gpu)
	{
		SurfaceSupportInfo surfaceSupport = {};

		if (testSurface == nullptr)
			return surfaceSupport;

		// -- CAPABILITIES --
		// Get the surface capabilities for the given physical pDevice
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, testSurface, &surfaceSupport.surfaceCapabilities);

		// -- FORMATS --
		// Retrieve the list of supported formats
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, testSurface, &formatCount, nullptr);
		if (formatCount > 0)
		{
			surfaceSupport.surfaceFormats.Resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, testSurface, &formatCount, surfaceSupport.surfaceFormats.GetData());
		}

		// -- PRESENTATION MODES --
		// Retrieve the list of supported presentation modes
		uint32_t presentationCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, testSurface, &presentationCount, nullptr);
		if (presentationCount > 0)
		{
			surfaceSupport.presentationModes.Resize(presentationCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, testSurface, &presentationCount, surfaceSupport.presentationModes.GetData());
		}

		return surfaceSupport;
	}

	void VulkanDevice::WaitUntilIdle()
	{
		vkDeviceWaitIdle(device);
	}

	VkFormat VulkanDevice::FindSupportedFormat(const Array<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (const auto& format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(gpu, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}
		return VK_FORMAT_UNDEFINED;
	}

	VkSurfaceFormatKHR VulkanDevice::FindAutoColorFormat()
	{
		bool formatSelected = false;
		const auto& surfaceFormats = surfaceSupport.surfaceFormats;
		VkSurfaceFormatKHR selectedSurfaceFormat{};

		if (surfaceFormats.GetSize() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			selectedSurfaceFormat = { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
			formatSelected = true;
		}
		else
		{
			// Find our preferred format first
			for (const auto& surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM)
				{
					selectedSurfaceFormat = { surfaceFormat.format, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
					formatSelected = true;
					break;
				}
			}

			if (!formatSelected)
			{
				// RGBA-8 is first preferred and BGRA-8 is second preferred format
				for (const auto& surfaceFormat : surfaceFormats)
				{
					if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM || surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
					{
						selectedSurfaceFormat = { surfaceFormat.format, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
						formatSelected = true;
						break;
					}
				}
			}
		}

		return surfaceFormats[0];
	}

	bool VulkanDevice::CheckSurfaceFormatSupport(VkFormat format)
	{
		const auto& surfaceFormats = surfaceSupport.surfaceFormats;

		for (const auto& surfaceFormat : surfaceFormats)
		{
			if (surfaceFormat.format == format)
			{
				return true;
			}
		}

		return false;
	}

	s32 VulkanDevice::FindMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(gpu, &memProperties);

		for (u32 i = 0; i < memProperties.memoryTypeCount; i++) 
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		return -1;
	}

	VkImageView VulkanDevice::CreateImageView(VkImage image, VkFormat format, VkImageViewType imageViewType, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo imageViewCI{};
		imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCI.image = image;
		imageViewCI.viewType = imageViewType;
		imageViewCI.format = format;

		// Allows remapping of RGBA components to other channel values
		imageViewCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		// Subresources allow the view to view only a part of an image
		imageViewCI.subresourceRange.aspectMask = aspectFlags;  // Which aspect of the image to view (e.g. color bit for viewing color)
		imageViewCI.subresourceRange.baseMipLevel = 0;          // Start mipmap level to view from
		imageViewCI.subresourceRange.levelCount = 1;            // No. of mipmap levels to view
		imageViewCI.subresourceRange.baseArrayLayer = 0;        // Start array layer to view from
		imageViewCI.subresourceRange.layerCount = 1;            // No. of array layers to view

		// CreateGraphicsPipeline image view and return it
		VkImageView imageView;
		if (vkCreateImageView(GetHandle(), &imageViewCI, nullptr, &imageView) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan Image View!");
			return nullptr;
		}
		return imageView;
	}

	void VulkanDevice::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask)
	{
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.aspectMask = aspectMask;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage = 0;
		VkPipelineStageFlags destinationStage = 0;

		switch (oldLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			barrier.srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			break;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
			break;
		default:
			return;
		}

		destinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		switch (newLayout)
		{
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			barrier.srcAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL || oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
				barrier.srcAccessMask |= VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			break;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			barrier.srcAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			barrier.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;
		default:
			return;
		}

		auto commandBuffer = BeginSingleUseCommandBuffer();

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		EndSingleUseCommandBuffer(commandBuffer);
		SubmitAndWaitSingleUseCommandBuffer(commandBuffer);
	}

	VkCommandBuffer VulkanDevice::BeginSingleUseCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = gfxCommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanDevice::EndSingleUseCommandBuffer(VkCommandBuffer commandBuffer)
	{
        vkEndCommandBuffer(commandBuffer);
	}

    void VulkanDevice::SubmitAndWaitSingleUseCommandBuffer(VkCommandBuffer commandBuffer)
    {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue->GetHandle(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue->GetHandle());

        vkFreeCommandBuffers(device, gfxCommandPool, 1, &commandBuffer);
    }

} // namespace CE
