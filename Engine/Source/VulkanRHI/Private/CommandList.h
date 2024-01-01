#pragma once

namespace CE::Vulkan
{

	class GraphicsCommandList : public RHI::GraphicsCommandList, public ApplicationMessageHandler
	{
	public:
		GraphicsCommandList(VulkanRHI* vulkanRHI, VulkanDevice* device, VulkanViewport* viewport);
		GraphicsCommandList(VulkanRHI* vulkanRHI, VulkanDevice* device, VulkanRenderTarget* renderTarget);
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

		virtual void CommitShaderResources(const Array<RHI::ShaderResourceGroup*>& shaderResourceGroups) override;

		virtual void SetRootConstants(u8 size, const u8* data) override;

		// TODO: Re-implement this function with more flexibilty. Maybe expose Fences?
		virtual void WaitForExecution() override;

		virtual void DrawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex, s32 vertexOffset, u32 firstInstance) override;

		// - Getters -

		CE_INLINE bool IsViewportTarget()
		{
			return viewport != nullptr;
		}

		CE_INLINE VulkanViewport* GetViewport()
		{
			return viewport;
		}

		CE_INLINE VulkanRenderTarget* GetRenderTarget()
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
		VulkanRenderTarget* renderTarget = nullptr;
		VulkanViewport* viewport = nullptr;
		ShaderResourceManager* srgManager = nullptr;
		VulkanPipelineLayout* currentPipelineLayout = nullptr;

		u32 numCommandBuffers = 1; // = BackBufferCount
		u32 simultaneousFrameDraws = 1;

		u32 currentImageIndex = 0;

		List<VkCommandBuffer> commandBuffers{};

		List<VkFence> renderFinishedFence{}; // Size = NumCommandBuffers
		List<VkSemaphore> renderFinishedSemaphore{}; // Size = NumCommandBuffers

		// DescriptorSet binding
		FixedArray<ShaderResourceGroup*, RHI::Limits::Pipeline::MaxShaderResourceGroupCount> shaderResourceGroupBindings{};
		u32 shaderResourceGroupBindingCount = 0;

		// ImGui
		b8 isImGuiEnabled = false;
		VkDescriptorPool imGuiDescriptorPool;

		friend class VulkanRHI;

	};
    
} // namespace CE::Vulkan
