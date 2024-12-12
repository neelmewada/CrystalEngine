#pragma once

namespace CE {
	class PlatformWindow;
}

namespace CE::RHI
{

	struct SwapChainDescriptor
	{
		//! @brief Number of images in the swapchain.
		u32 imageCount = 2;

		//! @brief Array of preferred image formats to use.
		Array<RHI::Format> preferredFormats = { RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM, };

		u32 preferredWidth = 0;
		u32 preferredHeight = 0;

		bool useMailboxMode = false;
	};
    
	class CORERHI_API SwapChain : public RHIResource
	{
	protected:
		SwapChain() : RHIResource(ResourceType::SwapChain)
		{}

	public:

		inline u32 GetCurrentImageIndex() const
		{
			return currentImageIndex;
		}

		inline Texture* GetCurrentImage() const
		{
			return images[currentImageIndex];
		}

		inline Texture* GetImage(u32 index) const { return images[index]; }

		inline u32 GetImageCount() const { return images.GetSize(); }

		inline u32 GetWidth() const { return width; }
		inline u32 GetHeight() const { return height; }

		inline u32 GetPreferredWidth() const { return width; }
		inline u32 GetPreferredHeight() const { return height; }

		inline f32 GetAspectRatio() const { return (f32)width / (f32)height; }

		inline RHI::Format GetSwapChainFormat() const { return swapChainColorFormat; }

		virtual PlatformWindow* GetNativeWindow() = 0;

		virtual void Rebuild() = 0;

	protected:

		Array<Texture*> images{};
		RHI::Format swapChainColorFormat{};

		u32 currentImageIndex = 0;
		u32 width = 0;
		u32 height = 0;

		u32 preferredWidth = 0;
		u32 preferredHeight = 0;
	};

} // namespace CE::RHI
