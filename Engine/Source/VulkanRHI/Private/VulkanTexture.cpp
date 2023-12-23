
#include "VulkanTexture.h"

#include "VulkanBuffer.h"


namespace CE
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

    VkFormat RHITextureFormatToVkFormat(RHI::TextureFormat format)
    {
		LoadMappings();

		if (!textureFormatToVkFormatMap.KeyExists(format))
			return VK_FORMAT_UNDEFINED;

		return textureFormatToVkFormatMap[format];
    }

    RHI::TextureFormat VkFormatToRHITextureFormat(VkFormat format)
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

	void VulkanRHI::Blit(RHI::Texture* source, RHI::Texture* destination, RHI::FilterMode filter)
	{
		if (source == nullptr || destination == nullptr)
			return;

		VulkanTexture* src = (VulkanTexture*)source;
		VulkanTexture* dst = (VulkanTexture*)destination;

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


    VulkanTexture::VulkanTexture(VulkanDevice* device, const RHI::TextureDesc& desc)
        : device(device)
    {
        this->name = desc.name;
        this->width = desc.width;
        this->height = desc.height;
        this->depth = desc.depth;

        this->dimension = desc.dimension;
        this->format = desc.format;
        this->mipLevels = desc.mipLevels;
        this->sampleCount = desc.sampleCount;
        
        VkImageCreateInfo imageCI{};
        imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        switch (desc.dimension)
        {
        case RHI::TextureDimension::Dim2D:
            imageCI.imageType = VK_IMAGE_TYPE_2D;
            break;
        case RHI::TextureDimension::Dim3D:
            imageCI.imageType = VK_IMAGE_TYPE_3D;
            break;
        case RHI::TextureDimension::Dim1D:
            imageCI.imageType = VK_IMAGE_TYPE_1D;
            break;
        case RHI::TextureDimension::DimCUBE:
            imageCI.imageType = VK_IMAGE_TYPE_2D;
        }
        
        imageCI.extent.width = width;
        imageCI.extent.height = height;
        imageCI.extent.depth = depth;

        imageCI.tiling = desc.forceLinearLayout ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
        imageCI.format = RHITextureFormatToVkFormat(this->format);
        imageCI.initialLayout = vkImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

        this->vkFormat = imageCI.format;
        
        if (EnumHasAnyFlags(desc.bindFlags, RHI::TextureBindFlags::ShaderRead))
        {
            imageCI.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
            aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
        }
        if (EnumHasFlag(desc.bindFlags, RHI::TextureBindFlags::Color))
        {
            imageCI.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
        }
        if (EnumHasFlag(desc.bindFlags, RHI::TextureBindFlags::DepthStencil))
        {
            imageCI.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
			if (this->format == RHI::TextureFormat::D24_UNORM_S8_UINT || this->format == RHI::TextureFormat::D32_SFLOAT_S8_UINT)
				aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
		if (EnumHasFlag(desc.bindFlags, RHI::TextureBindFlags::SubpassInput))
		{
			imageCI.usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		}
		
        imageCI.samples = (VkSampleCountFlagBits)desc.sampleCount;
        imageCI.mipLevels = mipLevels;
        imageCI.arrayLayers = 1;
        imageCI.flags = 0;

        imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCI.queueFamilyIndexCount = 0;
        imageCI.pQueueFamilyIndices = nullptr;
        
        if (vkCreateImage(device->GetHandle(), &imageCI, nullptr, &image) != VK_SUCCESS)
        {
            CE_LOG(Error, All, "Failed to create Vulkan Image");
            return;
        }

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
        case RHI::TextureDimension::Dim2D:
            imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
            break;
        case RHI::TextureDimension::Dim3D:
            imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_3D;
            break;
        case RHI::TextureDimension::Dim1D:
            imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_1D;
            break;
        case RHI::TextureDimension::DimCUBE:
            imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
            break;
        }

        // Subresources allow the view to view only a part of an image
        imageViewCI.subresourceRange.aspectMask = aspectMask;  // Which aspect of the image to view (e.g. color bit for viewing color)
        imageViewCI.subresourceRange.baseMipLevel = 0;          // Start mipmap level to view from
        imageViewCI.subresourceRange.levelCount = mipLevels;    // No. of mipmap levels to view
        imageViewCI.subresourceRange.baseArrayLayer = 0;        // Start array layer to view from
        imageViewCI.subresourceRange.layerCount = 1;            // No. of array layers to view

        if (vkCreateImageView(device->GetHandle(), &imageViewCI, nullptr, &imageView) != VK_SUCCESS)
        {
            CE_LOG(Error, All, "Failed to create Vulkan Image View");
            return;
        }

		// Transition image layout

		switch (desc.bindFlags)
		{
		case RHI::TextureBindFlags::ShaderRead:
			vkImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			break;
		case RHI::TextureBindFlags::Color:
			vkImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			break;
		case RHI::TextureBindFlags::DepthStencil:
			vkImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			break;
		default:
			break;
		}

		if (vkImageLayout != VK_IMAGE_LAYOUT_UNDEFINED)
			device->TransitionImageLayout(image, vkFormat, VK_IMAGE_LAYOUT_UNDEFINED, vkImageLayout, aspectMask);
    }

    VulkanTexture::~VulkanTexture()
    {
        vkDestroyImageView(device->GetHandle(), imageView, nullptr);
        imageView = nullptr;

        vkFreeMemory(device->GetHandle(), imageMemory, nullptr);
        imageMemory = nullptr;

        vkDestroyImage(device->GetHandle(), image, nullptr);
        image = nullptr;
    }

    u32 VulkanTexture::GetWidth()
    {
        return width;
    }

    u32 VulkanTexture::GetHeight()
    {
        return height;
    }

    u32 VulkanTexture::GetDepth()
    {
        return depth;
    }

    u32 VulkanTexture::GetBytesPerChannel()
    {
        u32 bytesPerChannel = 0;
        u32 numChannels = GetNumberOfChannelsForFormat(format, bytesPerChannel);
        return bytesPerChannel;
    }

    u32 VulkanTexture::GetNumberOfChannels()
    {
        u32 bytesPerChannel = 0;
        return GetNumberOfChannelsForFormat(format, bytesPerChannel);
    }

    void VulkanTexture::UploadData(const void* pixels, u64 dataSize)
    {
        u32 bytesPerChannel = 0;
        u32 numChannels = GetNumberOfChannelsForFormat(format, bytesPerChannel);
        u64 totalSize = (u64)width * (u64)height * (u64)depth * bytesPerChannel * numChannels;

		if (totalSize != dataSize)
		{
			totalSize = dataSize;
		}

		if (totalSize == 0)
			return;

        RHI::BufferData stagingBufferData{};
        stagingBufferData.startOffsetInBuffer = 0;
        stagingBufferData.data = pixels;
        stagingBufferData.dataSize = totalSize;

        RHI::BufferDesc stagingBufferDesc{};
        stagingBufferDesc.name = "Staging Texture Buffer";
        stagingBufferDesc.bindFlags = RHI::BufferBindFlags::StagingBuffer;
        stagingBufferDesc.allocMode = RHI::BufferAllocMode::SharedMemory;
        stagingBufferDesc.bindFlags = RHI::BufferUsageFlags::Default;
        stagingBufferDesc.bufferSize = totalSize;
        stagingBufferDesc.structureByteStride = bytesPerChannel * numChannels;
        stagingBufferDesc.initialData = &stagingBufferData;

        VulkanBuffer* stagingBuffer = new VulkanBuffer(device, stagingBufferDesc);

        device->TransitionImageLayout(image, vkFormat, vkImageLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aspectMask);
        CopyPixelsFromBuffer(stagingBuffer);
        device->TransitionImageLayout(image, vkFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vkImageLayout, aspectMask);

        delete stagingBuffer;
        stagingBuffer = nullptr;
    }

	void VulkanTexture::ReadData(u8** outPixels, u64* outDataSize)
	{
		if (outPixels == nullptr || outDataSize == nullptr)
			return;

		u32 bytesPerChannel = 0;
		u32 numChannels = GetNumberOfChannelsForFormat(format, bytesPerChannel);
		u64 totalSize = (u64)width * (u64)height * (u64)depth * bytesPerChannel * numChannels;

		if (totalSize == 0)
			return;

		RHI::BufferDesc stagingBufferDesc{};
		stagingBufferDesc.name = "Staging Texture Buffer";
		stagingBufferDesc.bindFlags = RHI::BufferBindFlags::StagingBuffer;
		stagingBufferDesc.allocMode = RHI::BufferAllocMode::SharedMemory;
		stagingBufferDesc.bindFlags = RHI::BufferUsageFlags::Default;
		stagingBufferDesc.bufferSize = totalSize;
		stagingBufferDesc.structureByteStride = bytesPerChannel * numChannels;
		stagingBufferDesc.initialData = nullptr;

		VulkanBuffer* stagingBuffer = new VulkanBuffer(device, stagingBufferDesc);

		device->TransitionImageLayout(image, vkFormat, vkImageLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, aspectMask);
		CopyPixelsToBuffer(stagingBuffer);
		device->TransitionImageLayout(image, vkFormat, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vkImageLayout, aspectMask);

		stagingBuffer->ReadData(outPixels, outDataSize);

		delete stagingBuffer;
		stagingBuffer = nullptr;
	}

    void VulkanTexture::CopyPixelsFromBuffer(VulkanBuffer* srcBuffer)
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

	void VulkanTexture::CopyPixelsToBuffer(VulkanBuffer* dstBuffer)
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
