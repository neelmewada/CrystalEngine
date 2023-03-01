
#include "VulkanDevice.h"
#include "PAL/Common/VulkanPlatform.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace CE
{

	VulkanDevice::VulkanDevice(VkInstance instance, NuklearVulkanRHI* vulkanRhi)
		: instance(instance), vulkanRhi(vulkanRhi)
	{
		
	}

	VulkanDevice::~VulkanDevice()
	{

	}

	void VulkanDevice::Initialize()
	{
		auto window = VulkanPlatform::GetActiveWindowHandle();
		testSurface = VulkanPlatform::CreateSurface(instance, window);

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

		vkDestroySurfaceKHR(instance, testSurface, nullptr);
		testSurface = nullptr;

		isInitialized = true;

		CE_LOG(Info, All, "Vulkan device initialized");
	}

	void VulkanDevice::PreShutdown()
	{
		isInitialized = false;

		delete graphicsQueue;
		delete presentQueue;
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

		vmaDestroyAllocator(vmaAllocator);
		vmaAllocator = nullptr;

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
		surfaceSupport = GetSurfaceSupportInfo(gpu);

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

		for (int i = 0; i < deviceExtensionCount; ++i)
		{
			// Required rule by Vulkan Specs.
			if (strcmp(deviceExtensionProperties[i].extensionName, "VK_KHR_portability_subset") == 0)
			{
				deviceExtensionNames.Add(deviceExtensionProperties[i].extensionName);
				break;
			}
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

		VmaAllocatorCreateInfo allocatorCI{};
		allocatorCI.instance = instance;
		allocatorCI.device = device;
		allocatorCI.physicalDevice = gpu;

		if (vmaCreateAllocator(&allocatorCI, &vmaAllocator) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan VmaAllocator!");
			return;
		}

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

		auto surfaceSupportInfo = GetSurfaceSupportInfo(gpu);
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
			if (queueFamilyProperty.queueCount > 0 &&
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
			if (!queueFamilies.IsValidPresentFamily() && queueFamilyProperty.queueCount > 0)
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

	SurfaceSupportInfo VulkanDevice::GetSurfaceSupportInfo(VkPhysicalDevice gpu)
	{
		SurfaceSupportInfo surfaceSupport = {};

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

} // namespace CE