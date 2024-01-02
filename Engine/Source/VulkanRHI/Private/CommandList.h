#pragma once

namespace CE::Vulkan
{

	class GraphicsCommandList : public RHI::GraphicsCommandList, public ApplicationMessageHandler
	{
	public:
		GraphicsCommandList(VulkanRHI* vulkanRHI, VulkanDevice* device, Viewport* viewport);
		GraphicsCommandList(VulkanRHI* vulkanRHI, VulkanDevice* device, RenderTarget* renderTarget);
		virtual ~GraphicsCommandList();

		// - Command List API -

		virtual void Begin() override;
		virtual void End() override;

		virtual void SetScissorRects(u32 scissorCount, const RHI::ScissorState* scissors) override;
		virtual void SetViewportRects(u32 viewportCount, const RHI::ViewportState* viewports) override;

		virtual void BindVertexBuffers(u32 firstBinding, const Array<RHI::Buffer*>& buffers) override;
		virtual void BindVertexBuffers(u32 firstBinding, const Array<RHI::Buffer*>& buffers, const Array<SIZE_T>& bufferOffsets) override;

		virtual void BindIndexBuffer(RHI::Buffer* buffer, bool use32BitIndex, SIZE_T offset) override;

		virtual void BindPipeline(RHI::IPipelineState* pipeline) override;

		virtual void SetShaderResourceGroups(const ArrayView<RHI::ShaderResourceGroup*>& shaderResourceGroups) override;

		virtual void CommitShaderResourceGroups() override;

		virtual void SetRootConstants(u8 size, const u8* data) override;

		// TODO: Re-implement this function with more flexibilty. Maybe expose Fences?
		virtual void WaitForExecution() override;

		virtual void DrawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex, s32 vertexOffset, u32 firstInstance) override;

		// - Getters -

		CE_INLINE bool IsViewportTarget()
		{
			return viewport != nullptr;
		}

		CE_INLINE Viewport* GetViewport()
		{
			return viewport;
		}

		CE_INLINE RenderTarget* GetRenderTarget()
		{
			return renderTarget;
		}

		void ProcessNativeEvents(void* nativeEvent) override;

		// - ImGui API -

		bool InitImGui(RHI::FontPreloadConfig* preloadFonts, Array<void*>& outFontHandles) override;

		void ShutdownImGui() override;

		virtual void ImGuiNewFrame() override;
		virtual void ImGuiRender() override;

		void ImGuiPlatformUpdate() override;

	protected:
		void CreateSyncObjects();
		void DestroySyncObjects();

	private:
		VulkanRHI* vulkanRHI = nullptr;
		VulkanDevice* device = nullptr;
		RenderTarget* renderTarget = nullptr;
		Viewport* viewport = nullptr;
		ShaderResourceManager* srgManager = nullptr;
		VulkanPipelineLayout* currentPipelineLayout = nullptr;

		u32 numCommandBuffers = 1; // = BackBufferCount
		u32 simultaneousFrameDraws = 1;

		u32 currentImageIndex = 0;

		List<VkCommandBuffer> commandBuffers{};

		List<VkFence> renderFinishedFence{}; // Size = NumCommandBuffers
		List<VkSemaphore> renderFinishedSemaphore{}; // Size = NumCommandBuffers

		// DescriptorSet binding
		StaticArray<ShaderResourceGroup*, RHI::Limits::Pipeline::MaxShaderResourceGroupCount> boundSRGs{};
		StaticArray<ShaderResourceGroup*, Vulkan::Limits::MaxDescriptorSetCount> commitedSRGsBySetNumber{};

		// ImGui
		b8 isImGuiEnabled = false;
		VkDescriptorPool imGuiDescriptorPool;

		friend class VulkanRHI;

	};
    
} // namespace CE::Vulkan
