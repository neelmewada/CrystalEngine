#pragma once

#include "VulkanRHIPrivate.h"

namespace CE
{
    class VulkanBuffer;

    class VulkanTexture : public RHI::Texture
    {
    public:
        VulkanTexture(VulkanDevice* device, const RHI::TextureDesc& desc);
        virtual ~VulkanTexture();

        virtual void* GetHandle() override
        {
            return image;
        }

        FORCE_INLINE VkImage GetImage() const
        {
            return image;
        }

        FORCE_INLINE VkImageView GetImageView() const
        {
            return imageView;
        }

		FORCE_INLINE VkImageLayout GetImageLayout() const
		{
			return vkImageLayout;
		}

		FORCE_INLINE VkImageAspectFlags GetAspectMask() const
		{
			return aspectMask;
		}

        virtual u32 GetWidth() override;
        virtual u32 GetHeight() override;
        virtual u32 GetDepth() override;
        virtual u32 GetBytesPerChannel() override;
        virtual u32 GetNumberOfChannels() override;

        virtual void UploadData(const void* pixels, u64 dataSize) override;

		virtual void ReadData(u8** outPixels, u64* outDataSize) override;

    protected:

        void CopyPixelsFromBuffer(VulkanBuffer* srcBuffer);

		void CopyPixelsToBuffer(VulkanBuffer* dstBuffer);

    private:
        VulkanDevice* device = nullptr;
        VkImage image = nullptr;
        VkDeviceMemory imageMemory = nullptr;
        VkImageView imageView = nullptr;
        RHI::TextureDimension dimension{};

        String name = "";
        u32 width = 0, height = 0, depth = 0;
        u32 sampleCount = 0;
        u32 mipLevels = 1;
        RHI::TextureFormat format{};
        VkFormat vkFormat{};
		VkImageLayout vkImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageAspectFlags aspectMask{};
    };
    
    VkFormat RHITextureFormatToVkFormat(RHI::TextureFormat format);
    RHI::TextureFormat VkFormatToRHITextureFormat(VkFormat format);
    u32 GetNumberOfChannelsForFormat(RHI::TextureFormat format, u32& outByteSizePerChannel);

} // namespace CE
