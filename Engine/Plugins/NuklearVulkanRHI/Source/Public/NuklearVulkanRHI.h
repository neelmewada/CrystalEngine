#pragma once

#include "CoreMinimal.h"
#include "RHI/RHI.h"

struct VkInstance_T;
typedef VkInstance_T* VkInstance;

struct VkDebugUtilsMessengerEXT_T;
typedef VkDebugUtilsMessengerEXT_T* VkDebugUtilsMessengerEXT;

struct VmaAllocator_T;
typedef VmaAllocator_T* VmaAllocator;

namespace CE
{
    class VulkanDevice;

    class NUKLEARVULKANRHI_API NuklearVulkanRHIModule : public PluginModule
    {
    public:
        NuklearVulkanRHIModule() {}
        virtual ~NuklearVulkanRHIModule() {}

        virtual void StartupModule() override;
        virtual void ShutdownModule() override;
        virtual void RegisterTypes() override;
        
    };

    class NUKLEARVULKANRHI_API NuklearVulkanRHI : public DynamicRHI
    {
    public:

        virtual void Initialize() override;
        virtual void PostInitialize() override;
        virtual void PreShutdown() override;
        virtual void Shutdown() override;
        
        virtual void GetNativeHandle(void** outInstance) override;
        
        virtual RHIGraphicsBackend GetGraphicsBackend() override;

    private:
        VkInstance vkInstance = nullptr;
        VkDebugUtilsMessengerEXT vkMessenger = nullptr;

        VulkanDevice* device = nullptr;
    };
    
} // namespace CE
