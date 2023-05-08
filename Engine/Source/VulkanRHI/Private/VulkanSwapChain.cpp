
#include "VulkanSwapChain.h"
#include "VulkanTexture.h"

#include "PAL/Common/VulkanPlatform.h"
#undef max

namespace CE
{

	VulkanSwapChain::VulkanSwapChain(VulkanRHI* vulkanRHI, void* windowHandle, VulkanDevice* device,
		u32 desiredBackBufferCount, u32 simultaneousFrameDraws, 
		u32 width, u32 height, bool isFullscreen, 
		RHIColorFormat colorFormat, RHIDepthStencilFormat depthBufferFormat)
		: vulkanRHI(vulkanRHI)
		, windowHandle(windowHandle)
		, device(device)
		, backBufferCount(desiredBackBufferCount)
		, simultaneousFrameDraws(simultaneousFrameDraws)
		, width(width)
		, height(height)
		, isFullscreen(isFullscreen)
		, colorBufferFormat(colorBufferFormat)
		, depthBufferFormat(depthBufferFormat)
	{
		
		CreateSurface();
		CreateSwapChain();
		CreateDepthBuffer();

		CE_LOG(Info, All, "Created Vulkan SwapChain");
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		DestroyDepthBuffer();

		for (const auto& swapChainImage : swapChainColorImages)
		{
			vkDestroyImageView(device->GetHandle(), swapChainImage.imageView, nullptr);
		}

		vkDestroySwapchainKHR(device->GetHandle(), swapChain, nullptr);

		vkDestroySurfaceKHR((VkInstance)vulkanRHI->GetNativeHandle(), surface, nullptr);

		CE_LOG(Info, All, "Destroyed Vulkan SwapChain");
	}

	void VulkanSwapChain::RebuildSwapChain()
	{
		CreateSwapChain();

		if (depthBufferFormat != RHIDepthStencilFormat::None)
		{
			DestroyDepthBuffer();
			CreateDepthBuffer();
		}
	}

	void VulkanSwapChain::CreateSurface()
	{
		surface = VulkanPlatform::CreateSurface((VkInstance)vulkanRHI->GetNativeHandle(), (PlatformWindow*)windowHandle);
	}

	void VulkanSwapChain::CreateSwapChain()
	{
		auto oldSwapChain = swapChain;
		auto gpu = device->GetPhysicalHandle();

		// -- SURFACE SUPPORT INFO --

		// Retrieve the list of supported formats
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr);

		Array<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, surfaceFormats.GetData());

		// Get the surface capabilities
		VkSurfaceCapabilitiesKHR surfaceCapabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surfaceCapabilities);

		// Select optimal surface format
		bool formatSelected = false;
		VkSurfaceFormatKHR selectedSurfaceFormat{};

		VkFormat preferredColorFormat = VK_FORMAT_R8G8B8A8_UNORM;
		if (colorBufferFormat == RHIColorFormat::BGRA32)
			preferredColorFormat = VK_FORMAT_B8G8R8A8_UNORM;

		if (surfaceFormats.GetSize() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			selectedSurfaceFormat = { preferredColorFormat, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
			formatSelected = true;
		}
		else
		{
			// Find our first preferred format
			for (const auto& format : surfaceFormats)
			{
				if (format.format == preferredColorFormat)
				{
					selectedSurfaceFormat = { format.format, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
					formatSelected = true;
					break;
				}
			}

			if (!formatSelected)
			{
				// RGBA-8 is first preferred and BGRA-8 is second preferred format
				for (const auto& format : surfaceFormats)
				{
					if (format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM)
					{
						selectedSurfaceFormat = { format.format, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
						formatSelected = true;
						break;
					}
				}
			}

			if (!formatSelected)
			{
				selectedSurfaceFormat = surfaceFormats[0];
				formatSelected = true;
			}
		}

		if (selectedSurfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM)
		{
			colorBufferFormat = RHIColorFormat::RGBA32;
		}
		else if (selectedSurfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
		{
			colorBufferFormat = RHIColorFormat::BGRA32;
		}
		else
		{
			colorBufferFormat = RHIColorFormat::Auto;
		}

		swapChainColorFormat = selectedSurfaceFormat;

		// - Select Extent

		VkExtent2D extent{};

		if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			extent = surfaceCapabilities.currentExtent;
		}
		else
		{
			extent.width = GetWidth();
			extent.height = GetHeight();

			// Surface also defines max & min. So make sure the values are clamped
			extent.width = Math::Clamp(extent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
			extent.height = Math::Clamp(extent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
		}

		this->width = extent.width;
		this->height = extent.height;

		// - Select a presentation mode

		// Retrieve the list of supported presentation modes
		uint32_t presentationCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentationCount, nullptr);

		Array<VkPresentModeKHR> presentationModes(presentationCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentationCount, presentationModes.GetData());

		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
		if (!presentationModes.Exists(VK_PRESENT_MODE_FIFO_KHR))
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

		u32 imageCount = backBufferCount;

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

		backBufferCount = imageCount;

		if (simultaneousFrameDraws > backBufferCount - 1)
			simultaneousFrameDraws = backBufferCount - 1;
		simultaneousFrameDraws = Math::Min(simultaneousFrameDraws, (u32)2); // Maximum 2 simultaneous draws

		// - Create SwapChain -
		VkSwapchainCreateInfoKHR swapChainCI{};
		swapChainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCI.surface = surface;
		swapChainCI.minImageCount = backBufferCount;
		swapChainCI.imageExtent = extent;
		swapChainCI.presentMode = presentMode;
		
		swapChainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCI.queueFamilyIndexCount = 0;
		swapChainCI.pQueueFamilyIndices = nullptr;

		swapChainCI.oldSwapchain = oldSwapChain;
		
		swapChainCI.preTransform = preTransform;
		swapChainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		swapChainCI.imageFormat = selectedSurfaceFormat.format;
		swapChainCI.imageColorSpace = selectedSurfaceFormat.colorSpace;
		swapChainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainCI.clipped = VK_TRUE;
		swapChainCI.imageArrayLayers = 1;
		
		if (vkCreateSwapchainKHR(device->GetHandle(), &swapChainCI, nullptr, &swapChain) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan SwapChain!");
			return;
		}

		if (oldSwapChain != nullptr)
		{
			vkDestroySwapchainKHR(device->GetHandle(), oldSwapChain, nullptr);
			oldSwapChain = nullptr;
		}

		// - Get SwapChain Images -
		u32 swapChainImageCount = 0;
		vkGetSwapchainImagesKHR(device->GetHandle(), swapChain, &swapChainImageCount, nullptr);
		std::vector<VkImage> images(swapChainImageCount);
		vkGetSwapchainImagesKHR(device->GetHandle(), swapChain, &swapChainImageCount, images.data());

		// Destroy old image views
		for (const auto& swapChainImage : swapChainColorImages)
		{
			vkDestroyImageView(device->GetHandle(), swapChainImage.imageView, nullptr);
		}

		swapChainColorImages.Clear();

		for (const auto& image : images)
		{
			VulkanSwapChainImage swapChainImage{};
			swapChainImage.image = image;
			swapChainImage.imageView = device->CreateImageView(image, swapChainColorFormat.format, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT);

			swapChainColorImages.Add(swapChainImage);
		}
	}

	void VulkanSwapChain::CreateDepthBuffer()
	{
		if (depthBufferFormat == RHIDepthStencilFormat::None)
		{
			return;
		}
		
		Array<VkFormat> preferredDepthFormats{};
		if (depthBufferFormat == RHIDepthStencilFormat::Auto || depthBufferFormat == RHIDepthStencilFormat::D32_S8)
			preferredDepthFormats.AddRange({ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT });
		else if (depthBufferFormat == RHIDepthStencilFormat::D24_S8)
			preferredDepthFormats.AddRange({ VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT });
		else if (depthBufferFormat == RHIDepthStencilFormat::D32)
			preferredDepthFormats.AddRange({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT });

		swapChainDepthFormat = device->FindSupportedFormat(preferredDepthFormats,
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		RHITextureFormat depthTextureFormat{};

		switch (swapChainDepthFormat)
		{
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			depthBufferFormat = RHIDepthStencilFormat::D32_S8;
			depthTextureFormat = RHITextureFormat::D32_SFLOAT_S8_UINT;
			break;
		case VK_FORMAT_D24_UNORM_S8_UINT:
			depthBufferFormat = RHIDepthStencilFormat::D24_S8;
			depthTextureFormat = RHITextureFormat::D24_UNORM_S8_UINT;
			break;
		case VK_FORMAT_D32_SFLOAT:
			depthBufferFormat = RHIDepthStencilFormat::D32;
			depthTextureFormat = RHITextureFormat::D32_SFLOAT;
			break;
		}
        
		RHITextureDesc depthTextureDesc{};
		depthTextureDesc.width = width;
		depthTextureDesc.height = height;
		depthTextureDesc.depth = 1;
		depthTextureDesc.dimension = RHITextureDimension::Dim2D;
		depthTextureDesc.format = depthTextureFormat;
		depthTextureDesc.mipLevels = 1;
		depthTextureDesc.sampleCount = 1;
		depthTextureDesc.usageFlags = RHITextureUsageFlags::DepthStencilAttachment;
		depthTextureDesc.forceLinearLayout = false;

		swapChainDepthImage = new VulkanTexture(device, depthTextureDesc);
	}

	void VulkanSwapChain::DestroyDepthBuffer()
	{
        vkDeviceWaitIdle(device->GetHandle());
        
		delete swapChainDepthImage;
	}

} // namespace CE

