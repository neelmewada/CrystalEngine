#pragma once

#include "CoreMinimal.h"
#include "RHI/RHI.h"

struct VkInstance_T;
typedef VkInstance_T* VkInstance;

struct VkDebugUtilsMessengerEXT_T;
typedef VkDebugUtilsMessengerEXT_T* VkDebugUtilsMessengerEXT;


namespace CE
{
    class VulkanDevice;
    class VulkanViewport;
    class VulkanGraphicsCommandList;
    class VulkanRenderTarget;

    class VULKANRHI_API VulkanRHIModule : public PluginModule
    {
    public:
        VulkanRHIModule() {}
        virtual ~VulkanRHIModule() {}

        virtual void StartupModule() override;
        virtual void ShutdownModule() override;
        virtual void RegisterTypes() override;
        
    };

    class VULKANRHI_API VulkanRHI : public DynamicRHI
    {
    public:
        virtual ~VulkanRHI() = default;

        virtual void Initialize() override;
        virtual void PostInitialize() override;
        virtual void PreShutdown() override;
        virtual void Shutdown() override;
        
        virtual void* GetNativeHandle() override;
        
        virtual RHIGraphicsBackend GetGraphicsBackend() override;

        // ************************************************
        // - Public API -

        // - Render Target -

        virtual RHIRenderTarget* CreateRenderTarget(u32 width, u32 height, 
            const RHIRenderTargetLayout& rtLayout) override;

        virtual void DestroyRenderTarget(RHIRenderTarget* renderTarget) override;

        virtual RHIViewport* CreateViewport(void* windowHandle,
            u32 width, u32 height, bool isFullscreen,
            const RHIRenderTargetLayout& rtLayout) override;

#if PAL_TRAIT_QT_SUPPORTED
        // - Qt Rendering API -
        virtual RHIViewport* CreateQtViewport(void* qtWindow, const RHIRenderTargetLayout& rtLayout) override;
#endif

        virtual void DestroyViewport(RHIViewport* viewport) override;

        // - Command List -
        virtual RHIGraphicsCommandList* CreateGraphicsCommandList(RHIViewport* viewport) override;

        virtual RHIGraphicsCommandList* CreateGraphicsCommandList(RHIRenderTarget* renderTarget) override;

        virtual void DestroyCommandList(RHICommandList* commandList) override;

        virtual void ExecuteCommandList(RHICommandList* commandList) override;

        virtual void PresentViewport(RHIGraphicsCommandList* viewportCommandList) override;

        // - Resources -

        virtual RHIBuffer* CreateBuffer(const RHIBufferDesc& bufferDesc) override;
        virtual void DestroyBuffer(RHIBuffer* buffer) override;

    protected:

        void ExecuteGraphicsCommandList(VulkanGraphicsCommandList* commandList, VulkanViewport* viewport);
        void ExecuteGraphicsCommandList(VulkanGraphicsCommandList* commandList, VulkanRenderTarget* renderTarget);

    private:
        VkInstance vkInstance = nullptr;
        VkDebugUtilsMessengerEXT vkMessenger = nullptr;

        VulkanDevice* device = nullptr;
    };
    
} // namespace CE
