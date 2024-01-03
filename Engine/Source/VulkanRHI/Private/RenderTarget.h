#pragma once

namespace CE::Vulkan
{
	class Texture;
	class VulkanFrameBuffer;
	class VulkanRenderPass;

	struct VulkanFrame
	{
		List<Texture*> textures{};
		List<VulkanSampler*> samplers{};
		VulkanFrameBuffer* framebuffer = nullptr;
	};

	/// Vulkan Render Target class
	class RenderTarget : public RHI::RenderTarget
	{
	public:
		/// An offscreen render target
		RenderTarget(VulkanDevice* device, const VulkanRenderTargetLayout& rtLayout);

		/// A viewport render target
		RenderTarget(VulkanDevice* device, Viewport* viewport, const VulkanRenderTargetLayout& rtLayout);

		virtual ~RenderTarget();

		virtual bool IsViewportRenderTarget() override { return isViewportRenderTarget; }

		virtual RHI::Viewport* GetRenderTargetViewport() override;

		virtual void SetClearColor(u32 colorTargetIndex, const Color& color) override;

		virtual void Resize(u32 newWidth, u32 newHeight) override;

		virtual int GetNumColorBuffers() override
		{
			return colorFrames.GetSize();
		}

		virtual RHI::Texture* GetColorTargetTexture(int index, int attachmentIndex = 0) override;
		virtual RHI::Sampler* GetColorTargetTextureSampler(int index, int attachmentIndex = 0) override;

		// - Getters -

		CE_INLINE VulkanRenderPass* GetVulkanRenderPass() { return renderPass; }

		CE_INLINE Viewport* GetVulkanViewport() { return viewport; }

		VkRenderPass GetVulkanRenderPassHandle() const;

		u32 GetBackBufferCount();

		u32 GetSimultaneousFrameDrawCount();

		virtual u32 GetWidth() override;

		virtual u32 GetHeight() override;

		u32 GetColorAttachmentCount()
		{
			return rtLayout.colorAttachmentCount;
		}

		u32 GetTotalAttachmentCount()
		{
			return rtLayout.attachmentDescCount;
		}

		bool HasDepthStencilAttachment() const
		{
			return rtLayout.HasDepthStencilAttachment();
		}


	protected:
		void CreateDepthBuffer();
		void DestroyDepthBuffer();
		void CreateColorBuffers();
		void DestroyColorBuffers();

	private:
		bool isViewportRenderTarget = false;
		Viewport* viewport = nullptr;
		bool isFresh = true;

		Color clearColors[RHI::Limits::Pipeline::MaxColorAttachmentCount] = {};
		VulkanDevice* device = nullptr;
		VulkanRenderTargetLayout rtLayout{};
		VulkanRenderPass* renderPass = nullptr;
		u32 backBufferCount = 0;
		u32 simultaneousFrameDraws = 0;

		u32 width = 0, height = 0;

		u32 currentDrawFrameIndex = 0;
		u32 currentImageIndex = 0;

		// Offscreen render targets
		Array<VulkanFrame> colorFrames{};
		VulkanFrame depthFrame{};

		friend class Viewport;
		friend class GraphicsCommandList;
		friend class VulkanFrameBuffer;
		friend class VulkanRHI;
		friend class GraphicsPipelineState;
	};
} // namespace CE
