#pragma once

namespace CE::RHI
{
	class RenderPass;
	class Sampler;
	class Texture;
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
    
} // namespace CE::RHI
