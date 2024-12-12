
#include "VulkanRHIPrivate.h"

#include "PAL/Common/VulkanPlatform.h"
#undef max

namespace CE::Vulkan
{
	SwapChain::SwapChain(VulkanRHI* rhi, VulkanDevice* device, PlatformWindow* window, const RHI::SwapChainDescriptor& desc)
		: rhi(rhi), device(device), window(window), desc(desc)
	{
		this->desc.preferredFormats.AddRange({ RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM });
		this->preferredWidth = desc.preferredWidth;
		this->preferredHeight = desc.preferredHeight;

		window->GetDrawableWindowSize(&width, &height);
		surface = VulkanPlatform::CreateSurface((VkInstance)rhi->GetNativeHandle(), window);

		if (preferredWidth != 0 && preferredHeight != 0)
		{
			f32 aspect = width / height;
			width = preferredWidth;
			height = preferredWidth / aspect;
		}

		Create();

		windowResizeCallback = PlatformApplication::Get()->onWindowDrawableSizeChanged.AddDelegateInstance(MemberDelegate(&SwapChain::OnWindowResized, this));
	}

	SwapChain::~SwapChain()
	{
		vkDeviceWaitIdle(device->GetHandle());

		// Delete images
		for (auto image : images)
		{
			delete image;
		}
		images.Clear();

		if (swapChain != nullptr)
		{
			vkDestroySwapchainKHR(device->GetHandle(), swapChain, VULKAN_CPU_ALLOCATOR);
			swapChain = nullptr;
		}

		if (surface != nullptr)
		{
			vkDestroySurfaceKHR(rhi->GetInstanceHandle(), surface, VULKAN_CPU_ALLOCATOR);
			surface = nullptr;
		}

		PlatformApplication::Get()->onWindowDrawableSizeChanged.RemoveDelegateInstance(windowResizeCallback);
		windowResizeCallback = 0;
	}

	void SwapChain::RebuildSwapChain()
	{
		ZoneScoped;

		window->GetDrawableWindowSize(&width, &height);

		if (preferredWidth != 0 && preferredHeight != 0)
		{
			f32 aspect = (f32)width / (f32)height;
			width = preferredWidth;
			height = (u32)((f32)preferredWidth / aspect);
		}

		Create();
	}

	void SwapChain::OnWindowResized(PlatformWindow* window, u32 newDrawWidth, u32 newDrawHeight)
	{
        if (this->window == window)
		{
			ZoneScoped;

            //RebuildSwapChain();
        }
	}

	void SwapChain::Create()
	{
		ZoneScoped;

		auto oldSwapChain = swapChain;
		auto gpu = device->GetPhysicalHandle();

		// -- SURFACE SUPPORT INFO --
		
		// Retrieve the list of supported formats
		u32 formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr);

		surfaceFormats.Resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, surfaceFormats.GetData());

		// Get the surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surfaceCapabilities);

		// Select optimal surface format
		bool formatSelected = false;
		VkSurfaceFormatKHR selectedSurfaceFormat{};

		// This condition means all formats are supported
		if (surfaceFormats.GetSize() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			formatSelected = true;
			if (desc.preferredFormats.IsEmpty())
			{
				selectedSurfaceFormat.format = VK_FORMAT_R8G8B8A8_UNORM;
				selectedSurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			}
			else
			{
				RHI::Format format = desc.preferredFormats[0];
				VkFormat vkFormat = RHIFormatToVkFormat(format);
				selectedSurfaceFormat.format = vkFormat;
				selectedSurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			}
		}
		else
		{
			// Find our first preferred format
			for (RHI::Format format : desc.preferredFormats)
			{
				VkFormat vkFormat = RHIFormatToVkFormat(format);
				if (vkFormat != VK_FORMAT_UNDEFINED &&
					surfaceFormats.Exists([&](const VkSurfaceFormatKHR& surfaceFormat) { return surfaceFormat.format == vkFormat; }))
				{
					formatSelected = true;
					selectedSurfaceFormat.format = vkFormat;
					selectedSurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
					break;
				}
			}

			// If we still couldn't select a format, just select the first available one.
			if (!formatSelected)
			{
				selectedSurfaceFormat = surfaceFormats[0];
				formatSelected = true;
			}
		}

		this->swapChainSurfaceFormat = selectedSurfaceFormat;

		swapChainColorFormat = VkFormatToRHIFormat(selectedSurfaceFormat.format);

		// - Select Extent

		VkExtent2D extent{};

		if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			extent = surfaceCapabilities.currentExtent;
		}
		else
		{
			extent.width = width;
			extent.height = height;

			// Surface also defines max & min values. So make sure the values are clamped
			extent.width = Math::Clamp(extent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
			extent.height = Math::Clamp(extent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
		}

		this->width = extent.width;
		this->height = extent.height;

		// - Select a presentation mode

		// Retrieve the list of supported presentation modes
		uint32_t presentationCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentationCount, nullptr);

		presentationModes.Resize(presentationCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentationCount, presentationModes.GetData());

		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
        //presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;

		if (desc.useMailboxMode && presentationModes.Exists(VK_PRESENT_MODE_MAILBOX_KHR))
		{
			presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		}

		this->presentMode = presentMode;

		// - Select PreTransform

		VkSurfaceTransformFlagBitsKHR preTransform{};

		if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		{
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else
		{
			preTransform = surfaceCapabilities.currentTransform;
		}

		// - Get Image count

		u32 imageCount = desc.imageCount;

		// If imageCount is higher than max allowed image count. If it's 0 then limitless
		if (surfaceCapabilities.maxImageCount > 0 &&
			surfaceCapabilities.maxImageCount < imageCount)
		{
			imageCount = surfaceCapabilities.maxImageCount;
		}
		// If imageCount is lower than min allowed image count. If it's 0 then limitless
		if (surfaceCapabilities.minImageCount > 0 &&
			surfaceCapabilities.minImageCount > imageCount)
		{
			imageCount = surfaceCapabilities.minImageCount;
		}

		desc.imageCount = imageCount;

		// - For triple buffering - (not supported for now)
		
		//if (simultaneousFramesInFlight > imageCount - 1)
		//	simultaneousFramesInFlight = imageCount - 1;
		//simultaneousFramesInFlight = Math::Min(simultaneousFramesInFlight, (u32)2); // Maximum 2 simultaneous draws
		simultaneousFramesInFlight = 1;

		// - Create SwapChain -
		VkSwapchainCreateInfoKHR swapChainCI{};
		swapChainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCI.surface = surface;
		swapChainCI.minImageCount = imageCount;
		swapChainCI.imageExtent = extent;
		swapChainCI.presentMode = presentMode;

		swapChainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCI.queueFamilyIndexCount = 0;
		swapChainCI.pQueueFamilyIndices = nullptr;

		// Facilitates quicker recreation of swapchain
		swapChainCI.oldSwapchain = oldSwapChain;
        
        swapChainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT)
        {
            swapChainCI.imageUsage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)
        {
            swapChainCI.imageUsage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        }
        if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
        {
            swapChainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }
        if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        {
            swapChainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }

		swapChainCI.preTransform = preTransform;
		swapChainCI.imageFormat = selectedSurfaceFormat.format;
		swapChainCI.imageColorSpace = selectedSurfaceFormat.colorSpace;
		swapChainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainCI.clipped = VK_TRUE;
		swapChainCI.imageArrayLayers = 1;

		if (vkCreateSwapchainKHR(device->GetHandle(), &swapChainCI, VULKAN_CPU_ALLOCATOR, &swapChain) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan SwapChain!");
			return;
		}

		// Destroy old Swap Chain if it exists
		if (oldSwapChain != nullptr)
		{
			vkDestroySwapchainKHR(device->GetHandle(), oldSwapChain, VULKAN_CPU_ALLOCATOR);
			oldSwapChain = nullptr;
		}

		// - Get SwapChain Images -
		u32 swapChainImageCount = 0;
		vkGetSwapchainImagesKHR(device->GetHandle(), swapChain, &swapChainImageCount, nullptr);
		List<VkImage> swapChainImages{}; swapChainImages.Resize(swapChainImageCount);
		vkGetSwapchainImagesKHR(device->GetHandle(), swapChain, &swapChainImageCount, swapChainImages.GetData());

		// Delete old images
		for (int i = 0; i < images.GetSize(); i++)
		{
			delete images[i];
		}
		images.Clear();

		// Create new images
		for (VkImage swapChainImage : swapChainImages)
		{
			RHI::ImageDescriptor imageDesc{};
			imageDesc.format = VkFormatToRHIFormat(swapChainSurfaceFormat.format);
			imageDesc.width = width;
			imageDesc.height = height;
			imageDesc.dimension = Dimension::Dim2D;
			imageDesc.depth = 1;
			imageDesc.name = "SwapChain";
			imageDesc.sampleCount = 1;
			imageDesc.bindFlags = RHI::TextureBindFlags::Color;

			Vulkan::Texture* image = new Vulkan::Texture(device, swapChainImage, imageDesc, VK_IMAGE_LAYOUT_UNDEFINED);
			images.Add(image);
		}

		for (int i = 0; i < swapChainInitialImageLayouts.GetSize(); i++)
		{
			swapChainInitialImageLayouts[i] = VK_IMAGE_LAYOUT_UNDEFINED;
		}
	}

} // namespace CE

