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
	};
    
	class CORERHI_API SwapChain : public RHIResource
	{
	protected:
		SwapChain() : RHIResource(ResourceType::SwapChain)
		{}

	public:


	protected:

		Array<Texture*> images{};

		u32 currentImageIndex = 0;

	};

} // namespace CE::RHI
