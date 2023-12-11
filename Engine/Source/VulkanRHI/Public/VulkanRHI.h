#pragma once

#include "CoreMinimal.h"
#include "CoreRHI.h"

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

    class VULKANRHI_API VulkanRHI : public RHI::DynamicRHI
    {
    public:
        virtual ~VulkanRHI() = default;

        virtual void Initialize() override;
        virtual void PostInitialize() override;
        virtual void PreShutdown() override;
        virtual void Shutdown() override;
        
        virtual void* GetNativeHandle() override;

        VkInstance GetInstanceHandle() const
        {
            return vkInstance;
        }
        
        virtual RHI::GraphicsBackend GetGraphicsBackend() override;

        // ************************************************
        // - Public API -

		// - Utils -

		// TODO: move this function to CoreApplication instead of VulkanRHI
		Vec2i GetScreenSizeForWindow(void* platformWindowHandle) override;

        // - Render Target -

        virtual RHI::RenderTarget* CreateRenderTarget(u32 width, u32 height,
            const RHI::RenderTargetLayout& rtLayout) override;

        virtual void DestroyRenderTarget(RHI::RenderTarget* renderTarget) override;

        virtual RHI::Viewport* CreateViewport(PlatformWindow* window,
            u32 width, u32 height, bool isFullscreen,
            const RHI::RenderTargetLayout& rtLayout) override;

        virtual void DestroyViewport(RHI::Viewport* viewport) override;

        // - Command List -
        virtual RHI::GraphicsCommandList* CreateGraphicsCommandList(RHI::Viewport* viewport) override;

        virtual RHI::GraphicsCommandList* CreateGraphicsCommandList(RHI::RenderTarget* renderTarget) override;

        virtual void DestroyCommandList(RHI::CommandList* commandList) override;

        virtual bool ExecuteCommandList(RHI::CommandList* commandList) override;

        virtual bool PresentViewport(RHI::GraphicsCommandList* viewportCommandList) override;

        // - Resources -

        virtual RHI::Buffer* CreateBuffer(const RHI::BufferDesc& bufferDesc) override;
        virtual void DestroyBuffer(RHI::Buffer* buffer) override;
        
        virtual RHI::Texture* CreateTexture(const RHI::TextureDesc& textureDesc) override;
        virtual void DestroyTexture(RHI::Texture* texture) override;
        
        virtual RHI::Sampler* CreateSampler(const RHI::SamplerDesc& samplerDesc) override;
        virtual void DestroySampler(RHI::Sampler* sampler) override;
        
        virtual void* AddImGuiTexture(RHI::Texture* texture, RHI::Sampler* sampler) override;
        virtual void RemoveImGuiTexture(void* imguiTexture) override; 

		virtual RHI::ShaderModule* CreateShaderModule(const RHI::ShaderModuleDesc& desc, const ShaderReflection& shaderReflection) override;
		virtual void DestroyShaderModule(RHI::ShaderModule* shaderModule) override;

		virtual RHI::ShaderResourceGroup* CreateShaderResourceGroup(const RHI::ShaderResourceGroupDesc& desc) override;
		virtual void DestroyShaderResourceGroup(RHI::ShaderResourceGroup* shaderResourceGroup) override;

		// - Pipeline State -

		virtual RHI::GraphicsPipelineState* CreateGraphicsPipelineState(RHI::RenderTarget* renderTarget, const RHI::GraphicsPipelineDesc& desc) override;
		virtual void DestroyPipelineState(RHI::IPipelineState* pipelineState) override;

		// - Utilities -

		virtual void Blit(RHI::Texture* source, RHI::Texture* destination, RHI::FilterMode filter) override;

    protected:

        bool ExecuteGraphicsCommandList(VulkanGraphicsCommandList* commandList, VulkanViewport* viewport);
        bool ExecuteGraphicsCommandList(VulkanGraphicsCommandList* commandList, VulkanRenderTarget* renderTarget);

    private:
        VkInstance vkInstance = nullptr;
        VkDebugUtilsMessengerEXT vkMessenger = nullptr;

        VulkanDevice* device = nullptr;
    };
    
} // namespace CE
