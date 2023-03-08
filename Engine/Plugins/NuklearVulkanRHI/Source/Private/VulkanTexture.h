#pragma once

#include "VulkanRHIPrivate.h"

namespace CE
{
    class VulkanTexture : public RHITexture
    {
    public:
        VulkanTexture(VulkanDevice* device, const RHITextureDesc& desc);
        virtual ~VulkanTexture();

        virtual void* GetHandle() override
        {
            return image;
        }

    private:
        VulkanDevice* device = nullptr;
        VkImage image = nullptr;
        VkImageView imageView = nullptr;
        RHITextureDimension dimension{};

        u32 width = 0, height = 0, depth = 0;
        u32 sampleCount = 0;
        u32 mipLevels = 1;
        RHITextureFormat format{};
        VkFormat vkFormat{};
    };
    
    VkFormat RHITextureFormatToVkFormat(RHITextureFormat format);

} // namespace CE
