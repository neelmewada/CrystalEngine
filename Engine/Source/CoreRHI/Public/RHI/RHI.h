#pragma once

#include "CoreMinimal.h"

#include "Resources.h"

namespace CE
{
    class PlatformWindow;
	struct ShaderReflection;
}

namespace CE::RHI
{
    class RenderTarget;
    class Viewport;
    class RenderPass;
    class CommandList;
    class GraphicsCommandList;

    class CORERHI_API DynamicRHI
    {
    public:
        
        virtual ~DynamicRHI() = default;
        
        // Lifecycle
        virtual void Initialize() = 0;
        virtual void PostInitialize() = 0;
        virtual void PreShutdown() = 0;
        virtual void Shutdown() = 0;
        
        virtual void* GetNativeHandle() = 0;

        virtual GraphicsBackend GetGraphicsBackend() = 0;

        // *******************************************
        // - Public API -

		// - Utils -

		/// Returns screen size for window based on which monitor it is in. Usually, platformWindowHandle is of type SDL_Window*.
		virtual Vec2i GetScreenSizeForWindow(void* platformWindowHandle) = 0;

        // - Render Target -

        virtual RenderTarget* CreateRenderTarget(u32 width, u32 height, 
            const RenderTargetLayout& rtLayout) = 0;

        virtual void DestroyRenderTarget(RenderTarget* renderTarget) = 0;

        virtual RHI::Viewport* CreateViewport(PlatformWindow* window,
            u32 width, u32 height, bool isFullscreen,
            const RHI::RenderTargetLayout& rtLayout) = 0;

        virtual void DestroyViewport(Viewport* viewport) = 0;

        // - Command List -

        virtual GraphicsCommandList* CreateGraphicsCommandList(Viewport* viewport) = 0;
        virtual GraphicsCommandList* CreateGraphicsCommandList(RenderTarget* renderTarget) = 0;
        virtual void DestroyCommandList(CommandList* commandList) = 0;

        virtual bool ExecuteCommandList(CommandList* commandList) = 0;

        virtual bool PresentViewport(GraphicsCommandList* viewportCommandList) = 0;

        // - Resources -

        virtual RHI::Buffer* CreateBuffer(const BufferDesc& bufferDesc) = 0;
        virtual void DestroyBuffer(Buffer* buffer) = 0;
        
        virtual RHI::Texture* CreateTexture(const TextureDesc& textureDesc) = 0;
        virtual void DestroyTexture(RHI::Texture* texture) = 0;
        
        virtual RHI::Sampler* CreateSampler(const SamplerDesc& samplerDesc) = 0;
        virtual void DestroySampler(Sampler* sampler) = 0;
        
        virtual void* AddImGuiTexture(RHI::Texture* texture, Sampler* sampler) = 0;
        virtual void RemoveImGuiTexture(void* imguiTexture) = 0;

		virtual RHI::ShaderModule* CreateShaderModule(const RHI::ShaderModuleDesc& desc, const ShaderReflection& shaderReflection) = 0;
		virtual void DestroyShaderModule(RHI::ShaderModule* shaderModule) = 0;

		virtual RHI::ShaderResourceGroup* CreateShaderResourceGroup(const RHI::ShaderResourceGroupDesc& desc) = 0;
		virtual void DestroyShaderResourceGroup(RHI::ShaderResourceGroup* shaderResourceGroup) = 0;

		// - Pipeline State -

		virtual RHI::GraphicsPipelineState* CreateGraphicsPipelineState(RHI::RenderTarget* renderTarget, const RHI::GraphicsPipelineDesc& desc) = 0;
		virtual void DestroyPipelineState(IPipelineState* pipelineState) = 0;

		// - Utilities -

		virtual void Blit(Texture* source, Texture* destination, FilterMode filter) = 0;

    };

    CORERHI_API extern DynamicRHI* gDynamicRHI;

    class CORERHI_API RenderPass : public Resource
    {
    protected:
        RenderPass() : Resource(ResourceType::RenderPass)
        {}
    public:
        virtual ~RenderPass() = default;

        // - Public API -

        

    };
    
} // namespace CE

