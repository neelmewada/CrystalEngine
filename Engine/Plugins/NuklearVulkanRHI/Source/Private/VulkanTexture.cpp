
#include "VulkanTexture.h"

namespace CE
{
    VkFormat RHITextureFormatToVkFormat(RHITextureFormat format)
    {
        switch (format)
        {
        case RHITextureFormat::R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case RHITextureFormat::R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case RHITextureFormat::R8G8B8A8_SNORM:
            return VK_FORMAT_R8G8B8A8_SNORM;
        }
        return VK_FORMAT_UNDEFINED;
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
        }
        
        imageCI.extent.width = width;
        imageCI.extent.height = height;
        imageCI.extent.depth = depth;

        imageCI.tiling = desc.forceLinearLayout ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
        imageCI.format = RHITextureFormatToVkFormat(this->format);
        imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCI.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        

    }

    VulkanTexture::~VulkanTexture()
    {

    }

} // namespace CE
