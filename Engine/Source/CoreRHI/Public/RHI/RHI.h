#pragma once

#include "CoreMinimal.h"

#include "RHIResources.h"

namespace CE
{
    class PlatformWindow;
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

		virtual RHI::ShaderModule* CreateShaderModule(const RHI::ShaderModuleDesc& desc) = 0;
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

	class Viewport;

    /// A render target that is drawn to by GPU. It is automatically created for you in case of Viewport.
    class CORERHI_API RenderTarget : public Resource
    {
    protected:
        RenderTarget() : Resource(ResourceType::RenderTarget)
        {}
    public:
        virtual ~RenderTarget() = default;

        // - Public API -

        virtual bool IsViewportRenderTarget() = 0;

		virtual RHI::Viewport* GetRenderTargetViewport() = 0;

        virtual RenderPass* GetRenderPass() = 0;

        virtual void SetClearColor(u32 colorTargetIndex, const Color& color) = 0;
        
		virtual void Resize(u32 newWidth, u32 newHeight) = 0;

		inline void Resize(Vec2i resolution)
		{
			Resize(resolution.x, resolution.y);
		}

		virtual u32 GetWidth() = 0;

		virtual u32 GetHeight() = 0;

		/// Number of back buffers used for rendering
		virtual int GetNumColorBuffers() = 0;

		virtual Texture* GetColorTargetTexture(int index, int attachmentIndex = 0) = 0;
		virtual Sampler* GetColorTargetTextureSampler(int index, int attachmentIndex = 0) = 0;
    };

    /// A viewport used to draw to & present from.
    class CORERHI_API Viewport : public Resource
    {
    protected:
        Viewport() : Resource(ResourceType::Viewport)
        {}
    public:
        virtual ~Viewport() = default;

        // - Public API -

        virtual RenderTarget* GetRenderTarget() = 0;

        virtual void SetClearColor(const Color& color) = 0;

        virtual void Rebuild() = 0;
        
        virtual void OnResize() = 0;
    };

    /*
    *   RHI Command List
    */

    class CORERHI_API CommandList : public Resource
    {
    protected:
        CommandList() : Resource(ResourceType::CommandList)
        {}
    public:
        virtual ~CommandList() = default;

        // - Public API -

        virtual CommandListType GetCommandListType() = 0;



    };

    class CORERHI_API GraphicsCommandList : public CommandList
    {
    protected:
        GraphicsCommandList() : CommandList()
        {}

    public:
        virtual ~GraphicsCommandList() = default;

        // - Public API -

        virtual CommandListType GetCommandListType() override final { return CommandListType::Graphics; }

        // - Command List API -

        virtual void Begin() = 0;
        virtual void End() = 0;

		virtual void BindVertexBuffers(u32 firstBinding, const Array<RHI::Buffer*>& buffers) = 0;
		virtual void BindVertexBuffers(u32 firstBinding, const Array<RHI::Buffer*>& buffers, const Array<SIZE_T>& bufferOffsets) = 0;

		virtual void BindIndexBuffer(RHI::Buffer* buffer, bool use32BitIndex, SIZE_T offset) = 0;

        // - ImGui Setup -

        virtual bool InitImGui(FontPreloadConfig* preloadFonts, Array<void*>& outFontHandles) = 0;

        virtual void ShutdownImGui() = 0;

        virtual void ImGuiNewFrame() = 0;

        virtual void ImGuiRender() = 0;

        virtual void ImGuiPlatformUpdate() = 0;

    };
    
} // namespace CE

