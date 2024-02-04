
#include "VulkanRHIPrivate.h"


namespace CE::Vulkan
{
	struct FormatEntry
	{
		RHI::TextureFormat rhiFormat = RHI::TextureFormat::Undefined;
		VkFormat vkFormat = VK_FORMAT_UNDEFINED;
		u32 numChannels = 0;
		u32 bytesPerChannel = 0;
	};

	static Array<FormatEntry> formatEntries{
		// RHI::TextureFormat, VkFormat, numChannels, bytesPerChannel
		{ RHI::TextureFormat::R8_UNORM, VK_FORMAT_R8_UNORM, 1, 1 },
		{ RHI::TextureFormat::R8_SNORM, VK_FORMAT_R8_SNORM, 1, 1 },
		{ RHI::TextureFormat::R8_SRGB, VK_FORMAT_R8_SRGB, 1, 1 },

		{ RHI::TextureFormat::R16G16_UNORM, VK_FORMAT_R16G16_UNORM, 2, 2 },
		{ RHI::TextureFormat::R16G16_SNORM, VK_FORMAT_R16G16_SNORM, 2, 2 },
		{ RHI::TextureFormat::R16G16_SINT, VK_FORMAT_R16G16_SINT, 2, 2 },
		{ RHI::TextureFormat::R16G16_SFLOAT, VK_FORMAT_R16G16_SFLOAT, 2, 2 },

		{ RHI::TextureFormat::R32G32_UINT, VK_FORMAT_R32G32_UINT, 2, 4 },
		{ RHI::TextureFormat::R32G32_SINT, VK_FORMAT_R32G32_SINT, 2, 4 },
		{ RHI::TextureFormat::R32G32_SFLOAT, VK_FORMAT_R32G32_SFLOAT, 2, 4 },

		{ RHI::TextureFormat::R8G8B8A8_SRGB, VK_FORMAT_R8G8B8A8_SRGB, 4, 1 },
		{ RHI::TextureFormat::R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, 4, 1 },
		{ RHI::TextureFormat::R8G8B8A8_SNORM, VK_FORMAT_R8G8B8A8_SNORM, 4, 1 },

		{ RHI::TextureFormat::B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_SRGB, 4, 1 },
		{ RHI::TextureFormat::B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM, 4, 1 },
		{ RHI::TextureFormat::B8G8R8A8_SNORM, VK_FORMAT_B8G8R8A8_SNORM, 4, 1 },

		{ RHI::TextureFormat::R8G8B8_UNORM, VK_FORMAT_R8G8B8_UNORM, 3, 1 },
		{ RHI::TextureFormat::R8G8B8_SNORM, VK_FORMAT_R8G8B8_SNORM, 3, 1 },
		{ RHI::TextureFormat::R8G8B8_SRGB, VK_FORMAT_R8G8B8_SRGB, 3, 1 },

		{ RHI::TextureFormat::R16_UNORM, VK_FORMAT_R16_UNORM, 1, 2 },
		{ RHI::TextureFormat::R16_SNORM, VK_FORMAT_R16_SNORM, 1, 2 },
		{ RHI::TextureFormat::R16_SFLOAT, VK_FORMAT_R16_SFLOAT, 1, 2 },
		{ RHI::TextureFormat::R32_UINT, VK_FORMAT_R32_UINT, 1, 4 },
		{ RHI::TextureFormat::R32_SINT, VK_FORMAT_R32_SINT, 1, 4 },
		{ RHI::TextureFormat::R32_SFLOAT, VK_FORMAT_R32_SFLOAT, 1, 4 },
		{ RHI::TextureFormat::D32_SFLOAT, VK_FORMAT_D32_SFLOAT, 1, 4 },
		{ RHI::TextureFormat::D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT, 1, 5 },
		{ RHI::TextureFormat::D24_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, 1, 4 },
		{ RHI::TextureFormat::D16_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, 1, 3 },

		{ RHI::TextureFormat::B8G8R8_UNORM, VK_FORMAT_B8G8R8_UNORM, 3, 1 },
		{ RHI::TextureFormat::B8G8R8_SNORM, VK_FORMAT_B8G8R8_SNORM, 3, 1 },
		{ RHI::TextureFormat::B8G8R8_SRGB, VK_FORMAT_B8G8R8_SRGB, 3, 1 },

		{ RHI::TextureFormat::BC7_UNORM, VK_FORMAT_BC7_UNORM_BLOCK, 4, 1 },
		{ RHI::TextureFormat::BC4_UNORM, VK_FORMAT_BC4_UNORM_BLOCK, 1, 1 },
	};

	static HashMap<RHI::TextureFormat, VkFormat> textureFormatToVkFormatMap{};
	static HashMap<VkFormat, RHI::TextureFormat> vkFormatToTextureFormatMap{};

	static void LoadMappings()
	{
		if (vkFormatToTextureFormatMap.GetSize() == 0)
		{
			for (const auto& entry : formatEntries)
			{
				textureFormatToVkFormatMap[entry.rhiFormat] = entry.vkFormat;
				vkFormatToTextureFormatMap[entry.vkFormat] = entry.rhiFormat;
			}
		}
	}

	VkFormat RHIFormatToVkFormat(RHI::TextureFormat format)
    {
		LoadMappings();

		if (!textureFormatToVkFormatMap.KeyExists(format))
			return VK_FORMAT_UNDEFINED;

		return textureFormatToVkFormatMap[format];
    }

	bool IsDepthVkFormat(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_X8_D24_UNORM_PACK32:
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
		case VK_FORMAT_D32_SFLOAT:
			return true;
		}
		return false;
	}

	bool IsStencilVkFormat(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_S8_UINT:
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return true;
		}
		return false;
	}

	RHI::TextureFormat VkFormatToRHIFormat(VkFormat format)
    {
		LoadMappings();

		if (!vkFormatToTextureFormatMap.KeyExists(format))
			return RHI::TextureFormat::Undefined;

		return vkFormatToTextureFormatMap[format];
    }

	u32 GetNumberOfChannelsForFormat(RHI::TextureFormat format, u32& outByteSizePerChannel)
    {
		LoadMappings();

		for (const auto& entry : formatEntries)
		{
			if (entry.rhiFormat == format)
			{
				outByteSizePerChannel = entry.bytesPerChannel;
				return entry.numChannels;
			}
		}

		return 0;
    }

	bool IsDepthFormat(RHI::Format format)
	{
		switch (format)
		{
		case RHI::Format::D16_UNORM_S8_UINT:
		case RHI::Format::D24_UNORM_S8_UINT:
		case RHI::Format::D32_SFLOAT_S8_UINT:
		case RHI::Format::D32_SFLOAT:
			return true;
		}
		return false;
	}

	bool IsDepthStencilFormat(RHI::Format format)
	{
		switch (format)
		{
		case RHI::Format::D16_UNORM_S8_UINT:
		case RHI::Format::D24_UNORM_S8_UINT:
		case RHI::Format::D32_SFLOAT_S8_UINT:
			return true;
		}
		return false;
	}

	void VulkanRHI::Blit(RHI::Texture* source, RHI::Texture* destination, RHI::FilterMode filter)
	{
		if (source == nullptr || destination == nullptr)
			return;

		Texture* src = (Texture*)source;
		Texture* dst = (Texture*)destination;

		VkFilter vkFilter = VK_FILTER_LINEAR;
		switch (filter)
		{
		case CE::RHI::FilterMode::Nearest:
			vkFilter = VK_FILTER_NEAREST;
			break;
		case CE::RHI::FilterMode::Cubic:
			if (device->IsDeviceExtensionSupported(VK_EXT_FILTER_CUBIC_EXTENSION_NAME))
				vkFilter = VK_FILTER_CUBIC_EXT;
			break;
		}

		VkCommandBuffer cmdBuffer = device->BeginSingleUseCommandBuffer();

		VkImageBlit region{};
		region.srcOffsets[0] = VkOffset3D{ 0, 0, 0 };
		region.srcOffsets[1] = VkOffset3D{ (int)src->GetWidth(), (int)src->GetHeight(), (int)src->GetDepth() };
		region.dstOffsets[0] = VkOffset3D{ 0, 0, 0 };
		region.dstOffsets[1] = VkOffset3D{ (int)dst->GetWidth(), (int)dst->GetHeight(), (int)dst->GetDepth() };

		region.srcSubresource.mipLevel = 0;
		region.srcSubresource.layerCount = 1;
		region.srcSubresource.aspectMask = src->GetAspectMask();
		region.srcSubresource.baseArrayLayer = 0;

		region.dstSubresource.mipLevel = 0;
		region.dstSubresource.layerCount = 1;
		region.dstSubresource.aspectMask = dst->GetAspectMask();
		region.dstSubresource.baseArrayLayer = 0;
		
		vkCmdBlitImage(cmdBuffer, src->GetImage(), src->GetImageLayout(), dst->GetImage(), dst->GetImageLayout(), 1, &region, vkFilter);

		device->EndSingleUseCommandBuffer(cmdBuffer);
		device->SubmitAndWaitSingleUseCommandBuffer(cmdBuffer);
	}

    void VulkanRHI::GetTextureMemoryRequirements(const RHI::TextureDescriptor& desc, ResourceMemoryRequirements& outRequirements)
    {
        VkImageCreateInfo imageCI{};
        imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        switch (desc.dimension)
        {
		case RHI::Dimension::Dim2D:
		case RHI::Dimension::DimCUBE:
            imageCI.imageType = VK_IMAGE_TYPE_2D;
            break;
        case RHI::Dimension::Dim3D:
            imageCI.imageType = VK_IMAGE_TYPE_3D;
            break;
        case RHI::Dimension::Dim1D:
            imageCI.imageType = VK_IMAGE_TYPE_1D;
            break;
        }
        
        imageCI.extent.width = desc.width;
        imageCI.extent.height = desc.height;
        imageCI.extent.depth = desc.depth;

		if (desc.defaultHeapType == RHI::MemoryHeapType::Default)
			imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
		else
			imageCI.tiling = VK_IMAGE_TILING_LINEAR;

        imageCI.format = RHIFormatToVkFormat(desc.format);
        imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        
        if (EnumHasAnyFlags(desc.bindFlags, RHI::TextureBindFlags::ShaderRead))
        {
            imageCI.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        if (EnumHasFlag(desc.bindFlags, RHI::TextureBindFlags::Color))
        {
            imageCI.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        if (EnumHasFlag(desc.bindFlags, RHI::TextureBindFlags::DepthStencil))
        {
            imageCI.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        if (EnumHasFlag(desc.bindFlags, RHI::TextureBindFlags::SubpassInput))
        {
            imageCI.usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        }
        
        imageCI.samples = (VkSampleCountFlagBits)desc.sampleCount;
        imageCI.mipLevels = desc.mipLevels;
        imageCI.arrayLayers = 1;
        imageCI.flags = 0;

        imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCI.queueFamilyIndexCount = 0;
        imageCI.pQueueFamilyIndices = nullptr;
        
        VkImage tempImage = nullptr;
        
        if (vkCreateImage(device->GetHandle(), &imageCI, nullptr, &tempImage) != VK_SUCCESS)
        {
            return;
        }
        
        VkMemoryRequirements memoryRequirements{};
        vkGetImageMemoryRequirements(device->GetHandle(), tempImage, &memoryRequirements);
        
        outRequirements.size = memoryRequirements.size;
        outRequirements.offsetAlignment = memoryRequirements.alignment;
        outRequirements.flags = memoryRequirements.memoryTypeBits;
        
        vkDestroyImage(device->GetHandle(), tempImage, nullptr);
    }

	void Texture::Init(const RHI::TextureDescriptor& desc)
	{
		this->name = desc.name;
		this->width = desc.width;
		this->height = desc.height;
		this->depth = desc.depth;
		this->arrayLayers = desc.arrayLayers;

		this->dimension = desc.dimension;
		this->format = desc.format;
		this->mipLevels = desc.mipLevels;
		this->sampleCount = desc.sampleCount;

		VkImageCreateInfo imageCI{};
		imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		switch (desc.dimension)
		{
		case RHI::Dimension::Dim2D:
		case RHI::Dimension::DimCUBE:
			imageCI.imageType = VK_IMAGE_TYPE_2D;
			break;
		case RHI::Dimension::Dim3D:
			imageCI.imageType = VK_IMAGE_TYPE_3D;
			break;
		case RHI::Dimension::Dim1D:
			imageCI.imageType = VK_IMAGE_TYPE_1D;
			break;
		}

		imageCI.extent.width = width;
		imageCI.extent.height = height;
		imageCI.extent.depth = depth;

		if (heapType == RHI::MemoryHeapType::Default)
			imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
		else
			imageCI.tiling = VK_IMAGE_TILING_LINEAR;

		imageCI.format = RHIFormatToVkFormat(this->format);
		imageCI.initialLayout = vkImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		curImageLayout = vkImageLayout;

		this->vkFormat = imageCI.format;

		bool isDepthFormat = IsDepthVkFormat(vkFormat);
		bool isStencilFormat = IsStencilVkFormat(vkFormat);
		if (isDepthFormat)
			aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
		if (isStencilFormat)
			aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		if (!isDepthFormat && !isStencilFormat)
			aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;

		if (EnumHasFlag(desc.bindFlags, RHI::TextureBindFlags::ShaderRead))
		{
			imageCI.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
		}
		if (EnumHasFlag(desc.bindFlags, RHI::TextureBindFlags::ShaderWrite))
		{
			imageCI.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
		}
		if (EnumHasFlag(desc.bindFlags, RHI::TextureBindFlags::Color))
		{
			imageCI.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}
		if (EnumHasFlag(desc.bindFlags, RHI::TextureBindFlags::DepthStencil))
		{
			imageCI.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		if (EnumHasFlag(desc.bindFlags, RHI::TextureBindFlags::Depth))
		{
			imageCI.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		if (EnumHasFlag(desc.bindFlags, RHI::TextureBindFlags::SubpassInput))
		{
			imageCI.usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		}

		imageCI.samples = (VkSampleCountFlagBits)desc.sampleCount;
		imageCI.mipLevels = mipLevels;
		imageCI.arrayLayers = arrayLayers;
		imageCI.flags = 0;

		imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCI.queueFamilyIndexCount = 0;
		imageCI.pQueueFamilyIndices = nullptr;

		if (arrayLayers == 6 && desc.dimension == RHI::Dimension::DimCUBE)
		{
			imageCI.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		}

		if (vkCreateImage(device->GetHandle(), &imageCI, nullptr, &image) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan Image");
			return;
		}
	}

	void Texture::AllocateInternal()
	{
		// Custom allocation
		VkMemoryRequirements memRequirements{};
		vkGetImageMemoryRequirements(device->GetHandle(), image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(device->GetHandle(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to allocate Vulkan Image Memory");
			return;
		}

		vkBindImageMemory(device->GetHandle(), image, imageMemory, 0);
	}

	void Texture::PostInit(const RHI::TextureDescriptor& desc)
	{
		VkImageViewCreateInfo imageViewCI{};
		imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCI.format = vkFormat;
		imageViewCI.image = image;

		imageViewCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		switch (dimension)
		{
		case RHI::Dimension::Dim2D:
			imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
			break;
		case RHI::Dimension::DimCUBE:
			imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
			break;
		case RHI::Dimension::Dim3D:
			imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_3D;
			break;
		case RHI::Dimension::Dim1D:
			imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_1D;
			break;
		}

		// Subresources allow the view to see only a part of an image
		imageViewCI.subresourceRange.aspectMask = aspectMask;   // Which aspect of the image to view (e.g. color bit for viewing color)
		imageViewCI.subresourceRange.baseMipLevel = 0;          // Start mipmap level to view from
		imageViewCI.subresourceRange.levelCount = mipLevels;    // No. of mipmap levels to view
		imageViewCI.subresourceRange.baseArrayLayer = 0;        // Start array layer to view from
		imageViewCI.subresourceRange.layerCount = arrayLayers;  // No. of array layers to view

		if (vkCreateImageView(device->GetHandle(), &imageViewCI, nullptr, &imageView) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan Image View");
			return;
		}

		// Transition image layout

		if (desc.bindFlags == RHI::TextureBindFlags::SubpassInput)
		{
			vkImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
		else if (desc.bindFlags == RHI::TextureBindFlags::Color)
		{
			vkImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		else if (desc.bindFlags == RHI::TextureBindFlags::DepthStencil)
		{
			vkImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		curImageLayout = vkImageLayout;

		if (vkImageLayout != VK_IMAGE_LAYOUT_UNDEFINED)
		{
			curFamilyIndex = device->TransitionImageLayout(image, vkFormat, VK_IMAGE_LAYOUT_UNDEFINED, vkImageLayout, imageViewCI.subresourceRange, aspectMask);
		}
	}

    Texture::Texture(VulkanDevice* device, const RHI::TextureDescriptor& desc)
        : device(device)
    {
		this->heapType = desc.defaultHeapType;
		Init(desc);
		AllocateInternal();
		PostInit(desc);
    }

	Texture::Texture(VulkanDevice* device, const RHI::TextureDescriptor& desc, const RHI::ResourceMemoryDescriptor& memoryDesc)
		: device(device)
	{
		if (memoryDesc.memoryHeap == nullptr)
		{
			this->heapType = desc.defaultHeapType;
			Init(desc);
			AllocateInternal();
			PostInit(desc);
			return;
		}

		this->heapType = memoryDesc.memoryHeap->GetHeapType();
		Init(desc);

		Vulkan::MemoryHeap* heap = (Vulkan::MemoryHeap*)memoryDesc.memoryHeap;

		auto result = vkBindImageMemory(device->GetHandle(), image, heap->GetHandle(), memoryDesc.memoryOffset);
		if (result != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to bind Memory to Texture {}", desc.name);
			return;
		}

		PostInit(desc);
	}

	Texture::Texture(VulkanDevice* device, VkImage image, const RHI::TextureDescriptor& desc, VkImageLayout dstLayout)
		: device(device)
		, importedImage(true)
		, image(image)
	{
		this->format = desc.format;
		this->vkFormat = RHIFormatToVkFormat(desc.format);
		this->name = desc.name;
		this->width = desc.width;
		this->height = desc.height;
		this->depth = desc.depth;
		this->arrayLayers = desc.arrayLayers;

		this->dimension = desc.dimension;
		this->format = desc.format;
		this->mipLevels = desc.mipLevels;
		this->sampleCount = desc.sampleCount;

		bool isDepthFormat = IsDepthVkFormat(vkFormat);
		bool isStencilFormat = IsStencilVkFormat(vkFormat);
		if (isDepthFormat)
			aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
		if (isStencilFormat)
			aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		if (!isDepthFormat && !isStencilFormat)
			aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;

		VkImageViewCreateInfo imageViewCI{};
		imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCI.image = image;
		switch (dimension)
		{
		case RHI::Dimension::Dim2D:
			imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
			break;
		case RHI::Dimension::DimCUBE:
			imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
			break;
		case RHI::Dimension::Dim3D:
			imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_3D;
			break;
		case RHI::Dimension::Dim1D:
			imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_1D;
			break;
		}
		imageViewCI.format = vkFormat;

		// Allows remapping of RGBA components to other channels
		imageViewCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		// Subresources allow the view to view only a part of an image
		imageViewCI.subresourceRange.aspectMask = aspectMask;  // Which aspect of the image to view (e.g. color bit for viewing color)
		imageViewCI.subresourceRange.baseMipLevel = 0;          // Start mipmap level to view from
		imageViewCI.subresourceRange.levelCount = 1;            // No. of mipmap levels to view
		imageViewCI.subresourceRange.baseArrayLayer = 0;        // Start array layer to view from
		imageViewCI.subresourceRange.layerCount = 1;            // No. of array layers to view

		if (vkCreateImageView(device->GetHandle(), &imageViewCI, nullptr, &imageView) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan Image View!");
			return;
		}

		if (dstLayout != VK_IMAGE_LAYOUT_UNDEFINED)
		{
			device->TransitionImageLayout(image, vkFormat, VK_IMAGE_LAYOUT_UNDEFINED, dstLayout, imageViewCI.subresourceRange, aspectMask);
			vkImageLayout = curImageLayout = dstLayout;
		}
	}

    Texture::~Texture()
    {
		if (name == "DepthStencil")
		{
			name.GetHashValue();
		}

        if (imageView != nullptr)
        {
            vkDestroyImageView(device->GetHandle(), imageView, nullptr);
            imageView = nullptr;
        }

        if (imageMemory != nullptr)
        {
            vkFreeMemory(device->GetHandle(), imageMemory, nullptr);
            imageMemory = nullptr;
        }

		// Don't destroy images that are managed externally
        if (!importedImage && image != nullptr)
        {
            vkDestroyImage(device->GetHandle(), image, nullptr);
            image = nullptr;
        }
    }

    u32 Texture::GetBytesPerChannel()
    {
        u32 bytesPerChannel = 0;
        u32 numChannels = GetNumberOfChannelsForFormat(format, bytesPerChannel);
        return bytesPerChannel;
    }

    u32 Texture::GetNumberOfChannels()
    {
        u32 bytesPerChannel = 0;
        return GetNumberOfChannelsForFormat(format, bytesPerChannel);
    }

    void Texture::CopyPixelsFromBuffer(Buffer* srcBuffer)
    {
        auto cmdBuffer = device->BeginSingleUseCommandBuffer();

        VkBuffer srcVkBuffer = srcBuffer->GetBuffer();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
		
        region.imageSubresource.aspectMask = aspectMask;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = {
            width,
            height,
            depth
        };

        vkCmdCopyBufferToImage(cmdBuffer, srcVkBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        device->EndSingleUseCommandBuffer(cmdBuffer);
        device->SubmitAndWaitSingleUseCommandBuffer(cmdBuffer);
    }

	void Texture::CopyPixelsToBuffer(Buffer* dstBuffer)
	{
		auto cmdBuffer = device->BeginSingleUseCommandBuffer();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = aspectMask;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			depth
		};
        
		vkCmdCopyImageToBuffer(cmdBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstBuffer->GetBuffer(), 1, &region);

		device->EndSingleUseCommandBuffer(cmdBuffer);
		device->SubmitAndWaitSingleUseCommandBuffer(cmdBuffer);
	}

} // namespace CE
