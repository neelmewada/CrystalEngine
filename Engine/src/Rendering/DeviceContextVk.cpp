
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "DeviceContextVk.h"
#include "GraphicsPipelineStateVk.h"
#include "ShaderVk.h"
#include "BufferVk.h"

#include <vulkan/vulkan.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vulkan/vulkan_beta.h>

#include "TypesVk.h"

#include <vector>
#include <iostream>
#include <set>

using namespace Vox;


DeviceContextVk::DeviceContextVk(DeviceCreateInfoVk& deviceInfo)
{
    m_EngineContext = dynamic_cast<EngineContextVk*>(deviceInfo.engineContext);
    if (m_EngineContext == nullptr)
    {
        throw std::runtime_error("ERROR: DeviceCreateInfoVk.engineContext is either NULL or not EngineContextVk.");
    }

    CreateSurface();
    FetchPhysicalDevice();
    // Save for later use
    m_SurfaceCompatInfo = FetchSurfaceCompatInfo(m_PhysicalDevice);
    CreateLogicalDevice();
    CreateVmaAllocator();
    CreateCommandPool();

    std::cout << "Created DeviceContextVk" << std::endl;
}

DeviceContextVk::~DeviceContextVk()
{
    vkDestroyCommandPool(m_Device, m_GraphicsCommandPool, nullptr);

    vmaDestroyAllocator(m_Allocator);

    vkDestroyDevice(m_Device, nullptr);

    vkDestroySurfaceKHR(m_EngineContext->GetInstance(), m_Surface, nullptr);

    std::cout << "Destroyed DeviceContextVk" << std::endl;
}

void DeviceContextVk::CreateSurface()
{
    auto result = SDL_Vulkan_CreateSurface(m_EngineContext->GetWindow(), m_EngineContext->GetInstance(), &m_Surface);
    if (result != SDL_TRUE)
    {
        throw std::runtime_error("Failed to create SDL Surface");
    }
}

void DeviceContextVk::FetchPhysicalDevice()
{
    auto instance = m_EngineContext->GetInstance();

    // Get the physical pDevice count first
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    if (physicalDeviceCount == 0)
    {
        throw std::runtime_error("Can't find a GPU that support Vulkan instances");
    }

    // Get the physical devices in an array
    auto physicalDevices = std::vector<VkPhysicalDevice>(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    VkPhysicalDevice selectedDevice = physicalDevices[0];
    VkPhysicalDeviceProperties selectedDeviceProps;
    VkPhysicalDeviceFeatures selectedDeviceFeatures;
    VkDeviceSize selectedDeviceMemorySize = 0;

    if (physicalDevices.size() == 1)
    {
        m_PhysicalDevice = physicalDevices[0];
        if (!CheckPhysicalDeviceSuitable(m_PhysicalDevice))
        {
            throw std::runtime_error("The physical pDevice does not have a Graphics queue and/or Compute queue.");
        }
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &selectedDeviceProps);
        std::cout << "Successfully selected the only GPU (" << selectedDeviceProps.deviceName << ") as the Physical Device." << std::endl;
        return;
    }

    vkGetPhysicalDeviceProperties(selectedDevice, &selectedDeviceProps);
    vkGetPhysicalDeviceFeatures(selectedDevice, &selectedDeviceFeatures);

    for (const auto& physicalDevice: physicalDevices)
    {
        // Info about the pDevice itself
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(physicalDevice, &props);

        // Info about what the pDevice can do
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physicalDevice, &features);

        // Select the best discrete gpu based on VRAM size
        auto memorySize = GetPhysicalDeviceLocalMemory(physicalDevice);

        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
            memorySize > selectedDeviceMemorySize)
        {
            if (CheckPhysicalDeviceSuitable(physicalDevice))
            {
                selectedDevice = physicalDevice;
                selectedDeviceProps = props;
                selectedDeviceFeatures = features;
                selectedDeviceMemorySize = memorySize;
            }
        }
    }

    m_PhysicalDevice = selectedDevice;
    if (!CheckPhysicalDeviceSuitable(m_PhysicalDevice))
    {
        throw std::runtime_error(std::string() +
                                 "The physical pDevice " + selectedDeviceProps.deviceName + " does not have a Graphics queue and/or Compute queue.");
    }

    std::cout << "Successfully selected " << selectedDeviceProps.deviceName << " as the Physical Device." << std::endl;
}

void DeviceContextVk::CreateLogicalDevice()
{
    QueueFamilyInfo queueFamilyInfo = FetchQueueFamilies(m_PhysicalDevice);

    if (!queueFamilyInfo.IsValidGraphicsFamily())
    {
        throw std::runtime_error("Failed to create Logical Device. Reason: Couldn't find a graphics queue family.");
    }
    if (!queueFamilyInfo.IsValidPresentationFamily())
    {
        throw std::runtime_error("Failed to create Logical Device. Reason: Couldn't find a queue that supports surfaces (presentation).");
    }

    m_QueueFamilies = queueFamilyInfo;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> queueFamilyIndices = { queueFamilyInfo.graphicsFamilyIndex, queueFamilyInfo.presentationFamilyIndex };

    for (const auto& queueFamilyIndex: queueFamilyIndices)
    {
        VkDeviceQueueCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        createInfo.queueFamilyIndex = queueFamilyIndex;
        createInfo.queueCount = 1;
        float priority = 1.0f; // Between 0.0 and 1.0
        createInfo.pQueuePriorities = &priority; // Priority if using multiple queues set by queueCount

        queueCreateInfos.push_back(createInfo);
    }

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    // Number of queues to create. 1 if both graphics & presentation queue are the same. 2 if they're different.
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    // Setup the enabled pDevice extensions
    auto deviceExtensionNames = std::vector<const char*>();
    uint32_t propCount = 0;
    vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &propCount, nullptr);
    auto extensionProps = std::vector<VkExtensionProperties>(propCount);
    vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &propCount, extensionProps.data());

    for (int i = 0; i < propCount; ++i)
    {
        // Required rule by Vulkan Specs.
        if (strcmp(extensionProps[i].extensionName, "VK_KHR_portability_subset") == 0)
        {
            deviceExtensionNames.push_back(extensionProps[i].extensionName);
            m_PortabilitySubsetEnabled = true;
            break;
        }
    }

    for (const auto &requiredExtension: k_RequiredDeviceExtensions)
    {
        deviceExtensionNames.push_back(requiredExtension);
    }

    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensionNames.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensionNames.size() > 0 ? deviceExtensionNames.data() : nullptr;

    // Physical Device Features the logical device will use
    VkPhysicalDeviceFeatures deviceFeatures = {};

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures; // Physical pDevice features the logical pDevice will use

    // Queues and logical pDevice are created together by this function
    VkResult result = vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create the Logical Device!");
    }

    // Get the graphics queue & presentation queue
    vkGetDeviceQueue(m_Device, queueFamilyInfo.graphicsFamilyIndex, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, queueFamilyInfo.presentationFamilyIndex, queueFamilyIndices.size() - 1, &m_PresentQueue);
}

void DeviceContextVk::CreateVmaAllocator()
{
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.device = m_Device;
    allocatorInfo.instance = m_EngineContext->GetInstance();
    allocatorInfo.physicalDevice = m_PhysicalDevice;
    auto result = vmaCreateAllocator(&allocatorInfo, &m_Allocator);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vma Allocator!");
    }
}

void DeviceContextVk::CreateCommandPool()
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = m_QueueFamilies.graphicsFamilyIndex;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    // CreateGraphicsPipeline a graphics queue family command pool
    auto result = vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_GraphicsCommandPool);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Graphics Family Command Pool");
    }
}

bool DeviceContextVk::CheckPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice)
{
    QueueFamilyInfo queueFamilyIndices = FetchQueueFamilies(physicalDevice);
    if (!queueFamilyIndices.IsValidGraphicsFamily() || !queueFamilyIndices.IsValidPresentationFamily())
        return false;

    bool extensionsSupported = CheckPhysicalDeviceExtensionSupport(physicalDevice);
    if (!extensionsSupported)
        return false;

    bool surfaceSupportsSwapChain = false;
    SurfaceCompatInfo surfaceCompatInfo = FetchSurfaceCompatInfo(physicalDevice);
    surfaceSupportsSwapChain = !surfaceCompatInfo.presentationModes.empty() &&
                               !surfaceCompatInfo.surfaceFormats.empty();
    if (!surfaceSupportsSwapChain)
        return false;

    return true;
}

bool DeviceContextVk::CheckPhysicalDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    if (extensionCount == 0)
        return false;

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());

    // Check for required extensions in the Physical pDevice extensions.
    for (const auto& requiredExtension: k_RequiredDeviceExtensions)
    {
        bool hasExtension = false;
        for (const auto& extension: extensions)
        {
            if (strcmp(extension.extensionName, requiredExtension) == 0)
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
    return true;
}

VkDeviceSize DeviceContextVk::GetPhysicalDeviceLocalMemory(VkPhysicalDevice physicalDevice)
{
    // Determine the available pDevice local memory.
    VkPhysicalDeviceMemoryProperties memoryProps;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProps);

    auto heapsPointer = memoryProps.memoryHeaps;
    auto heaps = std::vector<VkMemoryHeap>(heapsPointer, heapsPointer + memoryProps.memoryHeapCount);
    for (const auto& heap: heaps)
    {
        if (heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
        {
            return heap.size;
        }
    }
    return 0;
}

// It needs to have the surface set up to retrieve a presentation supported queue.
QueueFamilyInfo DeviceContextVk::FetchQueueFamilies(VkPhysicalDevice physicalDevice)
{
    QueueFamilyInfo queueFamilyInfo;

    // Get all queue family property count
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    auto queueFamilyList = std::vector<VkQueueFamilyProperties>(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyList.data());

    int curIndex = 0;

    // Go through each queue family and check if it has at least 1 queue in it and if it satisfies the requirements
    for (const auto& queueFamily: queueFamilyList)
    {
        // Select the first valid graphics queue family
        if (!queueFamilyInfo.IsValidGraphicsFamily() &&
            queueFamily.queueCount > 0 &&
            queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queueFamilyInfo.graphicsFamilyIndex = curIndex;
        }

        // Select the first valid queue that supports surfaces (presentation queue)
        // Prefer the graphics queue for presentation, if it supports it.
        if (physicalDevice != nullptr && m_Surface != nullptr)
        {
            VkBool32 presentationSupported = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, curIndex, m_Surface, &presentationSupported);
            if ((!queueFamilyInfo.IsValidPresentationFamily() || curIndex == queueFamilyInfo.graphicsFamilyIndex) &&
                queueFamily.queueCount > 0 &&
                presentationSupported)
            {
                queueFamilyInfo.presentationFamilyIndex = curIndex;
            }
        }

        // Select the first valid compute queue family
        if (!queueFamilyInfo.IsValidComputeFamily() &&
            queueFamily.queueCount > 0 &&
            queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            queueFamilyInfo.computeFamilyIndex = curIndex;
        }

        curIndex++;
    }

    return queueFamilyInfo;
}

SurfaceCompatInfo DeviceContextVk::FetchSurfaceCompatInfo(VkPhysicalDevice physicalDevice)
{
    SurfaceCompatInfo surfaceCompatInfo = {};

    // -- CAPABILITIES --
    // Get the surface capabilities for the given physical pDevice
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &surfaceCompatInfo.surfaceCapabilities);

    // -- FORMATS --
    // Retrieve the list of supported formats
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, nullptr);
    if (formatCount > 0)
    {
        surfaceCompatInfo.surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, surfaceCompatInfo.surfaceFormats.data());
    }

    // -- PRESENTATION MODES --
    // Retrieve the list of supported presentation modes
    uint32_t presentationCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentationCount, nullptr);
    if (presentationCount > 0)
    {
        surfaceCompatInfo.presentationModes.resize(presentationCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentationCount, surfaceCompatInfo.presentationModes.data());
    }

    return surfaceCompatInfo;
}

VkFormat DeviceContextVk::FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                              VkFormatFeatureFlags features)
{
    for (const auto& format: candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }
    throw std::runtime_error("Failed to find Supported VkFormat!");
}

#pragma region Public API

IGraphicsPipelineState* DeviceContextVk::CreateGraphicsPipelineState(const GraphicsPipelineStateCreateInfo &createInfo)
{
    return new GraphicsPipelineStateVk(createInfo);
}

IShader* DeviceContextVk::CreateShader(const ShaderCreateInfo& createInfo)
{
    return new ShaderVk(createInfo, this);
}

IBuffer* DeviceContextVk::CreateBuffer(const BufferCreateInfo& createInfo, BufferData &bufferData)
{
    return new BufferVk(createInfo, bufferData, this);
}

#pragma endregion
