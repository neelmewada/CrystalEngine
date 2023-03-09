#pragma once

#include "VulkanRHIPrivate.h"

namespace CE
{
    class VulkanBuffer;

    class VulkanTexture : public RHITexture
    {
    public:
        VulkanTexture(VulkanDevice* device, const RHITextureDesc& desc);
        virtual ~VulkanTexture();

        virtual void* GetHandle() override
        {
            return image;
        }

        CE_INLINE VkImage GetImage()
        {
            return image;
        }

        CE_INLINE VkImageView GetImageView()
        {
            return imageView;
        }

        virtual u32 GetWidth() override;
        virtual u32 GetHeight() override;
        virtual u32 GetDepth() override;
        virtual u32 GetBytesPerChannel() override;
        virtual u32 GetNumberOfChannels() override;

        virtual void UploadData(const void* pixels) override;

    protected:

        void CopyPixelsFromBuffer(VulkanBuffer* srcBuffer);

    private:
        VulkanDevice* device = nullptr;
        VkImage image = nullptr;
        VkDeviceMemory imageMemory = nullptr;
        VkImageView imageView = nullptr;
        RHITextureDimension dimension{};

        u32 width = 0, height = 0, depth = 0;
        u32 sampleCount = 0;
        u32 mipLevels = 1;
        RHITextureFormat format{};
        VkFormat vkFormat{};
        VkImageAspectFlags aspectMask{};
    };
    
    VkFormat RHITextureFormatToVkFormat(RHITextureFormat format);
    u32 GetNumberOfChannelsForFormat(RHITextureFormat format, u32& outByteSizePerChannel);

} // namespace CE
