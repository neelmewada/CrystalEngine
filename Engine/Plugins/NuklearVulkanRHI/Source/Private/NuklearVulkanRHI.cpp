
#include "CoreMinimal.h"

#include "NuklearVulkanRHI.h"
#include "VulkanRHIPrivate.h"
#include "PAL/Common/VulkanPlatform.h"

#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"

CE_IMPLEMENT_PLUGIN(NuklearVulkanRHI, CE::NuklearVulkanRHIModule)

namespace CE
{
	void NuklearVulkanRHIModule::StartupModule()
	{

	}

	void NuklearVulkanRHIModule::ShutdownModule()
	{

	}

	void NuklearVulkanRHIModule::RegisterTypes()
	{

	}

    VKAPI_ATTR VkBool32 VulkanValidationCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            CE_LOG(Error, All, "Vulkan Error: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            CE_LOG(Info, All, "Vulkan Info: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        {
            CE_LOG(Info, All, "Vulkan Verbose: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            CE_LOG(Warn, All, "Vulkan Warning: {}", pCallbackData->pMessage);
        }
        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) 
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) 
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else 
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        if (VulkanPlatform::IsVerboseValidationEnabled())
            createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = VulkanValidationCallback;
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) 
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) 
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

	void NuklearVulkanRHI::Initialize()
	{
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_API_VERSION_1_0;
        appInfo.pEngineName = CE_ENGINE_NAME_STRING;
        appInfo.engineVersion = VK_MAKE_VERSION(CE_VERSION_MAJOR, CE_VERSION_MINOR, CE_VERSION_PATCH);
        appInfo.pApplicationName = ProjectSettings::Get().GetProjectName().GetCString();
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        
        CE::Array<const char*> validationLayers = VulkanPlatform::GetValidationLayers();
        bool enableValidation = VulkanPlatform::IsValidationEnabled();
        CE::Array<const char*> instanceExtensions = VulkanPlatform::GetRequiredInstanceExtensions();

        VkInstanceCreateInfo instanceCI{};
        instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCI.pApplicationInfo = &appInfo;

        VkDebugUtilsMessengerCreateInfoEXT debugCI{};
        debugCI.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCI.pNext = nullptr;

        instanceCI.enabledExtensionCount = instanceExtensions.GetSize();
        instanceCI.ppEnabledExtensionNames = instanceExtensions.GetData();

        if (enableValidation)
        {
            instanceCI.enabledLayerCount = static_cast<uint32_t>(validationLayers.GetSize());
            instanceCI.ppEnabledLayerNames = validationLayers.GetData();

            PopulateDebugMessengerCreateInfo(debugCI);
            instanceCI.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCI;
        }
        else
        {
            instanceCI.enabledLayerCount = 0;
            instanceCI.ppEnabledLayerNames = nullptr;
            instanceCI.pNext = nullptr;
        }
        
        VkResult result = vkCreateInstance(&instanceCI, nullptr, &vkInstance);
        if (result != VK_SUCCESS)
        {
            CE_LOG(Critical, All, "Failed to create vulkan instance. Result = {}" , (int)result);
            return;
        }

        if (enableValidation && CreateDebugUtilsMessengerEXT(vkInstance, &debugCI, nullptr, &vkMessenger) != VK_SUCCESS)
        {
            CE_LOG(Error, All, "Failed to create Vulkan debug messenger!");
            return;
        }

        device = new VulkanDevice(vkInstance, this);
        device->Initialize();
	}

	void NuklearVulkanRHI::PreShutdown()
	{
        device->PreShutdown();
	}

	void NuklearVulkanRHI::Shutdown()
	{
        device->Shutdown();
        delete device;

        if (VulkanPlatform::IsValidationEnabled())
        {
            DestroyDebugUtilsMessengerEXT(vkInstance, vkMessenger, nullptr);
        }

        vkDestroyInstance(vkInstance, nullptr);
        vkInstance = nullptr;
	}

    void NuklearVulkanRHI::GetNativeHandle(void** outInstance)
    {
        *outInstance = vkInstance;
    }

	RHIGraphicsBackend NuklearVulkanRHI::GetGraphicsBackend()
	{
		return RHIGraphicsBackend::Vulkan;
	}

} // namespace CE
