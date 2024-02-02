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
	struct BufferDescriptor;

	enum class ValidationMessageType
	{
		Verbose = 0,
		Info,
		Warning,
		Error,
		COUNT
	};

	typedef void (*ValidationCallback)(ValidationMessageType, const char*);

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

		// - FrameGraph -

		virtual RHI::Scope* CreateScope(const ScopeDescriptor& desc) = 0;

		virtual RHI::FrameGraphCompiler* CreateFrameGraphCompiler() = 0;

		virtual RHI::FrameGraphExecuter* CreateFrameGraphExecuter() = 0;

		// - Utils -
        
		void AddValidationCallbackHandler(ValidationCallback handler, ValidationMessageType logLevel)
		{
			for (int i = (int)logLevel; i < validationCallbackHandlers.GetCapacity(); i++)
			{
				validationCallbackHandlers[i].Add(handler);
			}
		}

		void RemoveValidationCallbackHandler(ValidationCallback handler)
		{
			for (int i = 0; i < validationCallbackHandlers.GetCapacity(); i++)
			{
				validationCallbackHandlers[i].Remove(handler);
			}
		}

		virtual Array<RHI::Format> GetAvailableDepthStencilFormats() = 0;
		virtual Array<RHI::Format> GetAvailableDepthOnlyFormats() = 0;

		virtual Array<RHI::CommandQueue*> GetHardwareQueues(RHI::HardwareQueueClassMask queueMask) = 0;

		//! Returns true if RHI was initialized in Offscreen mode, i.e. there was no main window when RHI was initialized. 
		virtual bool IsOffscreenOnly() = 0;

		/// Returns screen size for window based on which monitor it is in. Usually, platformWindowHandle is of type SDL_Window*.
		virtual Vec2i GetScreenSizeForWindow(void* platformWindowHandle) = 0;

        // - Command List -

        virtual void DestroyCommandList(CommandList* commandList) = 0;

        virtual bool ExecuteCommandList(CommandList* commandList) = 0;

        virtual bool PresentViewport(GraphicsCommandList* viewportCommandList) = 0;

        // - Resources -

		virtual RHI::SwapChain* CreateSwapChain(PlatformWindow* window, const RHI::SwapChainDescriptor& desc) = 0;
		virtual void DestroySwapChain(RHI::SwapChain* swapChain) = 0;

		virtual RHI::MemoryHeap* AllocateMemoryHeap(const MemoryHeapDescriptor& desc) = 0;
		virtual void FreeMemoryHeap(RHI::MemoryHeap* memoryHeap) = 0;

		virtual void GetBufferMemoryRequirements(const BufferDescriptor& bufferDesc, ResourceMemoryRequirements& outRequirements) = 0;
        virtual void GetTextureMemoryRequirements(const TextureDescriptor& textureDesc, ResourceMemoryRequirements& outRequirements) = 0;

		virtual RHI::Buffer* CreateBuffer(const BufferDescriptor& bufferDesc) = 0;
		virtual RHI::Buffer* CreateBuffer(const BufferDescriptor& bufferDesc, const ResourceMemoryDescriptor& memoryDesc) = 0;
        virtual void DestroyBuffer(Buffer* buffer) = 0;
        
		virtual RHI::Texture* CreateTexture(const TextureDescriptor& textureDesc) = 0;
		virtual RHI::Texture* CreateTexture(const TextureDescriptor& textureDesc, const ResourceMemoryDescriptor& memoryDesc) = 0;
        virtual void DestroyTexture(RHI::Texture* texture) = 0;
        
        virtual RHI::Sampler* CreateSampler(const SamplerDescriptor& samplerDesc) = 0;
        virtual void DestroySampler(Sampler* sampler) = 0;
        
        virtual void* AddImGuiTexture(RHI::Texture* texture, Sampler* sampler) = 0;
        virtual void RemoveImGuiTexture(void* imguiTexture) = 0;

		virtual RHI::ShaderModule* CreateShaderModule(const RHI::ShaderModuleDescriptor& desc) = 0;
		virtual void DestroyShaderModule(RHI::ShaderModule* shaderModule) = 0;

		virtual RHI::ShaderResourceGroup* CreateShaderResourceGroup(const RHI::ShaderResourceGroupLayout& srgLayout) = 0;
		virtual void DestroyShaderResourceGroup(RHI::ShaderResourceGroup* shaderResourceGroup) = 0;

		// - Pipeline State -

		virtual RHI::PipelineState* CreateGraphicsPipeline(const RHI::GraphicsPipelineDescriptor& desc) = 0;
		virtual void DestroyPipeline(const RHI::PipelineState* pipeline) = 0;

		// - Helper Operations -

		virtual void Blit(Texture* source, Texture* destination, FilterMode filter) = 0;

		virtual u64 GetShaderStructMemberAlignment(const RHI::ShaderStructMember& member) = 0;
		virtual u64 GetShaderStructMemberSize(const RHI::ShaderStructMember& member) = 0;
		virtual void GetShaderStructMemberOffsets(const Array<RHI::ShaderStructMember>& members, Array<u64>& outOffsets) = 0;

	protected:

		StaticArray<Array<ValidationCallback>, (SIZE_T)ValidationMessageType::COUNT> validationCallbackHandlers{};
    };

    CORERHI_API extern DynamicRHI* gDynamicRHI;
    
} // namespace CE

