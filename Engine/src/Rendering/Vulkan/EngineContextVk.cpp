
#include "EngineContextVk.h"

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vector>
#include <iostream>

#pragma region DEBUGGING

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    //createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;
}

#pragma endregion

using namespace Vox;

namespace Vox
{
const std::vector<const char *> k_ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
};
const std::vector<const char *> k_RequiredDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
}

EngineContextVk::EngineContextVk(EngineCreateInfoVk& engineInfo)
{
    m_ValidationLayersEnabled = engineInfo.enableValidationLayers;

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pEngineName = ENGINE_NAME;
    appInfo.engineVersion = ENGINE_VERSION;
    appInfo.pApplicationName = engineInfo.applicationName;
    appInfo.applicationVersion = engineInfo.applicationVersion;
    appInfo.apiVersion = VK_API_VERSION_1_1; // Vulkan 1.1 required

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

    m_Window = ApplicationBase::Instance()->GetWindow();

    uint32_t sdlExtensionCount = 0;
    SDL_Vulkan_GetInstanceExtensions(m_Window, &sdlExtensionCount, nullptr);
    std::vector<const char*> sdlExtensionNames(sdlExtensionCount);
    SDL_Vulkan_GetInstanceExtensions(m_Window, &sdlExtensionCount, sdlExtensionNames.data());

    std::vector<const char*> instanceExtensions;

    for (int i = 0; i < sdlExtensionCount; ++i)
    {
        instanceExtensions.push_back(sdlExtensionNames[i]);
    }

    if (m_ValidationLayersEnabled)
    {
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    if (!CheckInstanceExtensionSupport(instanceExtensions))
    {
        throw std::runtime_error("VkInstance does not support required extensions!");
    }

    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (m_ValidationLayersEnabled)
    {
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(k_ValidationLayers.size());
        instanceInfo.ppEnabledLayerNames = k_ValidationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&instanceInfo, nullptr, &m_Instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan Instance!");
    }

    std::cout << "Successfully initialized Vulkan Instance!" << std::endl;

    if (m_ValidationLayersEnabled)
    {
        VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
        PopulateDebugMessengerCreateInfo(debugInfo);

        if (CreateDebugUtilsMessengerEXT(m_Instance, &debugInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("Failed to set up debug messenger!");
        }
    }

    std::cout << "Created EngineContext" << std::endl;
}

EngineContextVk::~EngineContextVk() noexcept
{
    if (m_ValidationLayersEnabled)
    {
        DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }

    vkDestroyInstance(m_Instance, nullptr);
    std::cout << "Destroyed EngineContext" << std::endl;
}

bool EngineContextVk::CheckInstanceExtensionSupport(std::vector<const char *>& checkExtensions)
{
    // Need to get number of extensions first
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    auto extensions = std::vector<VkExtensionProperties>(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    for (const auto checkExtension: checkExtensions)
    {
        bool hasExtension = false;
        for (int i = 0; i < extensionCount; ++i)
        {
            auto extension = extensions[i];
            if (strcmp(extension.extensionName, checkExtension))
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

