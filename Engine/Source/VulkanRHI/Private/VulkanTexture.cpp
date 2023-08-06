
#include "VulkanTexture.h"

#include "VulkanBuffer.h"


namespace CE
{
    VkFormat RHITextureFormatToVkFormat(RHI::TextureFormat format)
    {
        switch (format)
        {
        case RHI::TextureFormat::R8_UNORM:
            return VK_FORMAT_R8_UNORM;
        case RHI::TextureFormat::R8_SNORM:
            return VK_FORMAT_R8_SNORM;
        case RHI::TextureFormat::R8_SRGB:
            return VK_FORMAT_R8_SRGB;

		case RHI::TextureFormat::R16G16_UNORM:
			return VK_FORMAT_R16G16_UNORM;
		case RHI::TextureFormat::R16G16_SNORM:
			return VK_FORMAT_R16G16_SNORM;
		case RHI::TextureFormat::R16G16_UINT:
			return VK_FORMAT_R16G16_UINT;
		case RHI::TextureFormat::R16G16_SINT:
			return VK_FORMAT_R16G16_SINT;
		case RHI::TextureFormat::R16G16_SFLOAT:
			return VK_FORMAT_R16G16_SFLOAT;

		case RHI::TextureFormat::R32G32_UINT:
			return VK_FORMAT_R32G32_UINT;
		case RHI::TextureFormat::R32G32_SINT:
			return VK_FORMAT_R32G32_SINT;
		case RHI::TextureFormat::R32G32_SFLOAT:
			return VK_FORMAT_R32G32_SFLOAT;
            
        case RHI::TextureFormat::R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case RHI::TextureFormat::R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case RHI::TextureFormat::R8G8B8A8_SNORM:
            return VK_FORMAT_R8G8B8A8_SNORM;

        case RHI::TextureFormat::B8G8R8A8_SRGB:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case RHI::TextureFormat::B8G8R8A8_UNORM:
            return VK_FORMAT_B8G8R8A8_UNORM;
        case RHI::TextureFormat::B8G8R8A8_SNORM:
            return VK_FORMAT_B8G8R8A8_SNORM;

        case RHI::TextureFormat::R8G8B8_UNORM:
            return VK_FORMAT_R8G8B8_UNORM;
        case RHI::TextureFormat::R8G8B8_SNORM:
            return VK_FORMAT_R8G8B8_SNORM;
        case RHI::TextureFormat::R8G8B8_SRGB:
            return VK_FORMAT_R8G8B8_SRGB;
        case RHI::TextureFormat::R16_UNORM:
            return VK_FORMAT_R16_UNORM;
        case RHI::TextureFormat::R16_SNORM:
            return VK_FORMAT_R16_SNORM;
        case RHI::TextureFormat::R16_SFLOAT:
            return VK_FORMAT_R16_SFLOAT;
        case RHI::TextureFormat::R32_UINT:
            return VK_FORMAT_R32_UINT;
        case RHI::TextureFormat::R32_SINT:
            return VK_FORMAT_R32_SINT;
        case RHI::TextureFormat::R32_SFLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case RHI::TextureFormat::D32_SFLOAT:
            return VK_FORMAT_D32_SFLOAT;
        case RHI::TextureFormat::D32_SFLOAT_S8_UINT:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case RHI::TextureFormat::D24_UNORM_S8_UINT:
            return VK_FORMAT_D24_UNORM_S8_UINT;
                
        case RHI::TextureFormat::B8G8R8_UNORM:
            return VK_FORMAT_B8G8R8_UNORM;
        case RHI::TextureFormat::B8G8R8_SNORM:
            return VK_FORMAT_B8G8R8_SNORM;
        case RHI::TextureFormat::B8G8R8_SRGB:
            return VK_FORMAT_B8G8R8_SRGB;
        }
        
        return VK_FORMAT_UNDEFINED;
    }

    RHI::TextureFormat VkFormatToRHITextureFormat(VkFormat format)
    {
        switch (format)
        {
        case VK_FORMAT_R8_UNORM:
            return RHI::TextureFormat::R8_UNORM;
        case VK_FORMAT_R8_SNORM:
            return RHI::TextureFormat::R8_SNORM;
        case VK_FORMAT_R8_SRGB:
            return RHI::TextureFormat::R8_SRGB;

        case VK_FORMAT_R8G8B8A8_SRGB:
            return RHI::TextureFormat::R8G8B8A8_SRGB;
        case VK_FORMAT_R8G8B8A8_UNORM:
            return RHI::TextureFormat::R8G8B8A8_UNORM;
        case VK_FORMAT_R8G8B8A8_SNORM:
            return RHI::TextureFormat::R8G8B8A8_SNORM;

		case VK_FORMAT_R16G16_UNORM:
			return RHI::TextureFormat::R16G16_UNORM;
		case VK_FORMAT_R16G16_SNORM:
			return RHI::TextureFormat::R16G16_SNORM;
		case VK_FORMAT_R16G16_UINT:
			return RHI::TextureFormat::R16G16_UINT;
		case VK_FORMAT_R16G16_SINT:
			return RHI::TextureFormat::R16G16_SINT;
		case VK_FORMAT_R16G16_SFLOAT:
			return RHI::TextureFormat::R16G16_SFLOAT;

        case VK_FORMAT_B8G8R8A8_SRGB:
            return RHI::TextureFormat::B8G8R8A8_SRGB;
        case VK_FORMAT_B8G8R8A8_UNORM:
            return RHI::TextureFormat::B8G8R8A8_UNORM;
        case VK_FORMAT_B8G8R8A8_SNORM:
            return RHI::TextureFormat::B8G8R8A8_SNORM;

        case VK_FORMAT_R8G8B8_UNORM:
            return RHI::TextureFormat::R8G8B8_UNORM;
        case VK_FORMAT_R8G8B8_SNORM:
            return RHI::TextureFormat::R8G8B8_SNORM;
        case VK_FORMAT_R8G8B8_SRGB:
            return RHI::TextureFormat::R8G8B8_SRGB;

        case VK_FORMAT_R16_UNORM:
            return RHI::TextureFormat::R16_UNORM;
        case VK_FORMAT_R16_SNORM:
            return RHI::TextureFormat::R16_SNORM;
        case VK_FORMAT_R16_SFLOAT:
            return RHI::TextureFormat::R16_SFLOAT;

        case VK_FORMAT_R32_UINT:
            return RHI::TextureFormat::R32_UINT;
        case VK_FORMAT_R32_SINT:
            return RHI::TextureFormat::R32_SINT;
        case VK_FORMAT_R32_SFLOAT:
            return RHI::TextureFormat::R32_SFLOAT;

		case VK_FORMAT_R32G32_UINT:
			return RHI::TextureFormat::R32G32_UINT;
		case VK_FORMAT_R32G32_SINT:
			return RHI::TextureFormat::R32G32_SINT;
		case VK_FORMAT_R32G32_SFLOAT:
			return RHI::TextureFormat::R32G32_SFLOAT;

        case VK_FORMAT_D32_SFLOAT:
            return RHI::TextureFormat::D32_SFLOAT;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return RHI::TextureFormat::D32_SFLOAT_S8_UINT;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return RHI::TextureFormat::D24_UNORM_S8_UINT;
                
        case VK_FORMAT_B8G8R8_UNORM:
            return RHI::TextureFormat::B8G8R8_UNORM;
        case VK_FORMAT_B8G8R8_SNORM:
            return RHI::TextureFormat::B8G8R8_SNORM;
        case VK_FORMAT_B8G8R8_SRGB:
            return RHI::TextureFormat::B8G8R8_SRGB;
        }
        
        return RHI::TextureFormat::Undefined;
    }

    u32 GetNumberOfChannelsForFormat(RHI::TextureFormat format, u32& outByteSizePerChannel)
    {
		VK_FORMAT_R32G32_SFLOAT;
        switch (format)
        {
        case RHI::TextureFormat::R8_UNORM:
            outByteSizePerChannel = 1;
            return 1;
        case RHI::TextureFormat::R8_SNORM:
            outByteSizePerChannel = 1;
            return 1;
        case RHI::TextureFormat::R8_SRGB:
            outByteSizePerChannel = 1;
            return 1;
		
		case RHI::TextureFormat::B8G8R8A8_SRGB:
			outByteSizePerChannel = 1;
			return 4;
		case RHI::TextureFormat::B8G8R8A8_UNORM:
			outByteSizePerChannel = 1;
			return 4;
		case RHI::TextureFormat::B8G8R8A8_SNORM:
			outByteSizePerChannel = 1;
			return 4;

        case RHI::TextureFormat::R8G8B8A8_SRGB:
            outByteSizePerChannel = 1;
            return 4;
        case RHI::TextureFormat::R8G8B8A8_UNORM:
            outByteSizePerChannel = 1;
            return 4;
        case RHI::TextureFormat::R8G8B8A8_SNORM:
            outByteSizePerChannel = 1;
            return 4;

        case RHI::TextureFormat::R8G8B8_UNORM:
            outByteSizePerChannel = 1;
            return 3;
        case RHI::TextureFormat::R8G8B8_SNORM:
            outByteSizePerChannel = 1;
            return 3;
        case RHI::TextureFormat::R8G8B8_SRGB:
            outByteSizePerChannel = 1;
            return 3;

		case RHI::TextureFormat::R16G16_UNORM:
			outByteSizePerChannel = 2;
			return 2;
		case RHI::TextureFormat::R16G16_SNORM:
			outByteSizePerChannel = 2;
			return 2;
		case RHI::TextureFormat::R16G16_UINT:
			outByteSizePerChannel = 2;
			return 2;
		case RHI::TextureFormat::R16G16_SINT:
			outByteSizePerChannel = 2;
			return 2;
		case RHI::TextureFormat::R16G16_SFLOAT:
			outByteSizePerChannel = 2;
			return 2;

		case RHI::TextureFormat::R32G32_UINT:
			outByteSizePerChannel = 4;
			return 2;
		case RHI::TextureFormat::R32G32_SINT:
			outByteSizePerChannel = 4;
			return 2;
		case RHI::TextureFormat::R32G32_SFLOAT:
			outByteSizePerChannel = 4;
			return 2;

        case RHI::TextureFormat::R16_UNORM:
            outByteSizePerChannel = 2;
            return 1;
        case RHI::TextureFormat::R16_SNORM:
            outByteSizePerChannel = 2;
            return 1;
        case RHI::TextureFormat::R16_SFLOAT:
            outByteSizePerChannel = 2;
            return 1;
        case RHI::TextureFormat::R32_UINT:
            outByteSizePerChannel = 4;
            return 1;
        case RHI::TextureFormat::R32_SINT:
            outByteSizePerChannel = 4;
            return 1;
        case RHI::TextureFormat::R32_SFLOAT:
            outByteSizePerChannel = 4;
            return 1;
        case RHI::TextureFormat::D32_SFLOAT:
            outByteSizePerChannel = 4;
            return 1;
        case RHI::TextureFormat::D32_SFLOAT_S8_UINT:
            outByteSizePerChannel = 5;
            return 1;
        case RHI::TextureFormat::D24_UNORM_S8_UINT:
            outByteSizePerChannel = 4;
            return 1;
        }

        return 0;
    }

	VkFormat CMImageFormatToVkFormat(CMImageFormat format, u32 bitDepth, u32 bitsPerPixel)
	{
		switch (format)
		{
		case CE::CMImageFormat::R:
			if (bitDepth == 32)
				return VK_FORMAT_R32_SFLOAT;
			else if (bitDepth == 16)
				return VK_FORMAT_R16_UNORM;
			else if (bitDepth == 8)
				return VK_FORMAT_R8_UNORM;
			break;
		case CE::CMImageFormat::RG:
			if (bitDepth == 32)
				return VK_FORMAT_R32G32_SFLOAT;
			else if (bitDepth == 16)
				return VK_FORMAT_R16G16_UNORM;
			else if (bitDepth == 8)
				return VK_FORMAT_R8G8_UNORM;
			break;
		case CE::CMImageFormat::RGB:
			if (bitDepth == 32)
				return VK_FORMAT_R32G32B32_SFLOAT;
			else if (bitDepth == 16)
				return VK_FORMAT_R16G16B16_UNORM;
			else if (bitDepth == 8)
				return VK_FORMAT_R8G8B8_UNORM;
			break;
		case CE::CMImageFormat::RGBA:
			if (bitDepth == 32)
				return VK_FORMAT_R32G32B32A32_SFLOAT;
			else if (bitDepth == 16)
				return VK_FORMAT_R16G16B16A16_UNORM;
			else if (bitDepth == 8)
				return VK_FORMAT_R8G8B8A8_UNORM;// Transcode: BC7_RGBA
			break;
		default:
			break;
		}

		return VK_FORMAT_UNDEFINED;
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
        imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

        this->vkFormat = imageCI.format;
        
        if (EnumHasAnyFlags(desc.usageFlags, RHI::TextureUsageFlags::SampledImage))
        {
            imageCI.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
            aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
        }
        if (EnumHasFlag(desc.usageFlags, RHI::TextureUsageFlags::ColorAttachment))
        {
            imageCI.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
        }
        if (EnumHasFlag(desc.usageFlags, RHI::TextureUsageFlags::DepthStencilAttachment))
        {
            imageCI.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
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

    void VulkanTexture::UploadData(const void* pixels)
    {
        u32 bytesPerChannel = 0;
        u32 numChannels = GetNumberOfChannelsForFormat(format, bytesPerChannel);
        u64 totalSize = (u64)width * (u64)height * (u64)depth * bytesPerChannel * numChannels;

        RHI::BufferData stagingBufferData{};
        stagingBufferData.startOffsetInBuffer = 0;
        stagingBufferData.data = pixels;
        stagingBufferData.dataSize = totalSize;

        RHI::BufferDesc stagingBufferDesc{};
        stagingBufferDesc.name = "Staging Texture Buffer";
        stagingBufferDesc.bindFlags = RHI::BufferBindFlags::StagingBuffer;
        stagingBufferDesc.allocMode = RHI::BufferAllocMode::SharedMemory;
        stagingBufferDesc.usageFlags = RHI::BufferUsageFlags::Default;
        stagingBufferDesc.bufferSize = totalSize;
        stagingBufferDesc.structureByteStride = bytesPerChannel * numChannels;
        stagingBufferDesc.initialData = &stagingBufferData;

        VulkanBuffer* stagingBuffer = new VulkanBuffer(device, stagingBufferDesc);

        device->TransitionImageLayout(image, vkFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aspectMask);
        CopyPixelsFromBuffer(stagingBuffer);
        device->TransitionImageLayout(image, vkFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, aspectMask);

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

} // namespace CE
