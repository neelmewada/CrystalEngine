#pragma once

#include "VulkanRHIPrivate.h"

#include "vulkan/vulkan.h"

namespace CE {
	class PlatformWindow;
}

namespace CE::Vulkan
{
    class VulkanDevice;
    class Texture;

    struct VulkanSwapChainImage
    {
        VkImage image = nullptr;
        VkImageView imageView = nullptr;
    };

	class SwapChain : public RHI::SwapChain
	{
	public:
		SwapChain(VulkanRHI* rhi, VulkanDevice* device, PlatformWindow* window, const RHI::SwapChainDescriptor& desc);

		virtual ~SwapChain();

		void RebuildSwapChain();

	protected:

		void OnWindowResized(PlatformWindow* window, u32 newDrawWidth, u32 newDrawHeight);

		void Create();

		VulkanRHI* rhi = nullptr;
		VulkanDevice* device = nullptr;
		PlatformWindow* window = nullptr;

		RHI::SwapChainDescriptor desc{};

		List<VkSurfaceFormatKHR> surfaceFormats{};
		VkSurfaceCapabilitiesKHR surfaceCapabilities{};
		List<VkPresentModeKHR> presentationModes{};

		//! @brief We do not support triple buffering, etc for time being.
		u32 simultaneousFramesInFlight = 1;

		VkSwapchainKHR swapChain = nullptr;
		VkSurfaceKHR surface = nullptr;

		VkPresentModeKHR presentMode{};
		VkSurfaceFormatKHR swapChainSurfaceFormat{};
		RHI::Format swapChainColorFormat{};

		u32 width = 0;
		u32 height = 0;

		DelegateHandle windowResizeCallback = 0;
	};

} // namespace CE
