
#include "VulkanTexture.h"

#include "VulkanBuffer.h"

namespace CE
{
    VkFormat RHITextureFormatToVkFormat(RHITextureFormat format)
    {
        switch (format)
        {
        case RHITextureFormat::R8_UNORM:
            return VK_FORMAT_R8_UNORM;
        case RHITextureFormat::R8_SNORM:
            return VK_FORMAT_R8_SNORM;
        case RHITextureFormat::R8_SRGB:
            return VK_FORMAT_R8_SRGB;
        case RHITextureFormat::R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case RHITextureFormat::R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case RHITextureFormat::R8G8B8A8_SNORM:
            return VK_FORMAT_R8G8B8A8_SNORM;
        case RHITextureFormat::R8G8B8_UNORM:
            return VK_FORMAT_R8G8B8_UNORM;
        case RHITextureFormat::R8G8B8_SNORM:
            return VK_FORMAT_R8G8B8_SNORM;
        case RHITextureFormat::R8G8B8_SRGB:
            return VK_FORMAT_R8G8B8_SRGB;
        case RHITextureFormat::R16_UNORM:
            return VK_FORMAT_R16_UNORM;
        case RHITextureFormat::R16_SNORM:
            return VK_FORMAT_R16_SNORM;
        case RHITextureFormat::R16_SFLOAT:
            return VK_FORMAT_R16_SNORM;
        case RHITextureFormat::R32_UINT:
            return VK_FORMAT_R32_UINT;
        case RHITextureFormat::R32_SINT:
            return VK_FORMAT_R32_SINT;
        case RHITextureFormat::R32_SFLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case RHITextureFormat::D32_SFLOAT:
            return VK_FORMAT_D32_SFLOAT;
        case RHITextureFormat::D32_SFLOAT_S8_UINT:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case RHITextureFormat::D24_UNORM_S8_UINT:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        }
        
        return VK_FORMAT_UNDEFINED;
    }

    u32 GetNumberOfChannelsForFormat(RHITextureFormat format, u32& outByteSizePerChannel)
    {
        switch (format)
        {
        case RHITextureFormat::R8_UNORM:
            outByteSizePerChannel = 1;
            return 1;
        case RHITextureFormat::R8_SNORM:
            outByteSizePerChannel = 1;
            return 1;
        case RHITextureFormat::R8_SRGB:
            outByteSizePerChannel = 1;
            return 1;
        case RHITextureFormat::R8G8B8A8_SRGB:
            outByteSizePerChannel = 1;
            return 4;
        case RHITextureFormat::R8G8B8A8_UNORM:
            outByteSizePerChannel = 1;
            return 4;
        case RHITextureFormat::R8G8B8A8_SNORM:
            outByteSizePerChannel = 1;
            return 4;
        case RHITextureFormat::R8G8B8_UNORM:
            outByteSizePerChannel = 1;
            return 3;
        case RHITextureFormat::R8G8B8_SNORM:
            outByteSizePerChannel = 1;
            return 3;
        case RHITextureFormat::R8G8B8_SRGB:
            outByteSizePerChannel = 1;
            return 3;
        case RHITextureFormat::R16_UNORM:
            outByteSizePerChannel = 2;
            return 1;
        case RHITextureFormat::R16_SNORM:
            outByteSizePerChannel = 2;
            return 1;
        case RHITextureFormat::R16_SFLOAT:
            outByteSizePerChannel = 2;
            return 1;
        case RHITextureFormat::R32_UINT:
            outByteSizePerChannel = 4;
            return 1;
        case RHITextureFormat::R32_SINT:
            outByteSizePerChannel = 4;
            return 1;
        case RHITextureFormat::R32_SFLOAT:
            outByteSizePerChannel = 4;
            return 1;
        case RHITextureFormat::D32_SFLOAT:
            outByteSizePerChannel = 4;
            return 1;
        case RHITextureFormat::D32_SFLOAT_S8_UINT:
            outByteSizePerChannel = 5;
            return 1;
        case RHITextureFormat::D24_UNORM_S8_UINT:
            outByteSizePerChannel = 4;
            return 1;
        }

        return 0;
    }

    VulkanTexture::VulkanTexture(VulkanDevice* device, const RHITextureDesc& desc)
        : device(device)
    {
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
        case RHITextureDimension::Dim2D:
            imageCI.imageType = VK_IMAGE_TYPE_2D;
            break;
        case RHITextureDimension::Dim3D:
            imageCI.imageType = VK_IMAGE_TYPE_3D;
            break;
        case RHITextureDimension::Dim1D:
            imageCI.imageType = VK_IMAGE_TYPE_1D;
            break;
        case RHITextureDimension::DimCUBE:
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
        
        if (EnumHasFlag(desc.usageFlags, RHITextureUsageFlags::SampledImage))
        {
            imageCI.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
            aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
        }
        if (EnumHasFlag(desc.usageFlags, RHITextureUsageFlags::ColorAttachment))
        {
            imageCI.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
        }
        if (EnumHasFlag(desc.usageFlags, RHITextureUsageFlags::DepthStencilAttachment))
        {
            imageCI.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }

        imageCI.samples = (VkSampleCountFlagBits)desc.sampleCount;
        imageCI.flags = 0;
        
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
        case RHITextureDimension::Dim2D:
            imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
            break;
        case RHITextureDimension::Dim3D:
            imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_3D;
            break;
        case RHITextureDimension::Dim1D:
            imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_1D;
            break;
        case RHITextureDimension::DimCUBE:
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

        RHIBufferData stagingBufferData{};
        stagingBufferData.startOffsetInBuffer = 0;
        stagingBufferData.data = pixels;
        stagingBufferData.dataSize = totalSize;

        RHIBufferDesc stagingBufferDesc{};
        stagingBufferDesc.name = "Staging Texture Buffer";
        stagingBufferDesc.bindFlags = RHIBufferBindFlags::StagingBuffer;
        stagingBufferDesc.allocMode = RHIBufferAllocMode::SharedMemory;
        stagingBufferDesc.usageFlags = RHIBufferUsageFlags::Default;
        stagingBufferDesc.bufferSize = totalSize;
        stagingBufferDesc.structureByteStride = bytesPerChannel * numChannels;

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
    }

} // namespace CE
