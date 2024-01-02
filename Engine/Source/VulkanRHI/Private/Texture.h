#pragma once

namespace CE::Vulkan
{
	class VulkanDevice;

    class Texture : public RHI::Texture
    {
    public:
        Texture(VulkanDevice* device, const RHI::TextureDescriptor& desc);
        virtual ~Texture();

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
        
        virtual u32 GetBytesPerChannel() override;
        virtual u32 GetNumberOfChannels() override;

        virtual void UploadData(const void* pixels, u64 dataSize) override;

		virtual void ReadData(u8** outPixels, u64* outDataSize) override;

    protected:

        void CopyPixelsFromBuffer(Buffer* srcBuffer);

		void CopyPixelsToBuffer(Buffer* dstBuffer);

    private:
        VulkanDevice* device = nullptr;
        VkImage image = nullptr;
        VkDeviceMemory imageMemory = nullptr;
        VkImageView imageView = nullptr;
        
        VkFormat vkFormat{};
		VkImageLayout vkImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageAspectFlags aspectMask{};
    };
    
    VkFormat RHITextureFormatToVkFormat(RHI::TextureFormat format);
    RHI::TextureFormat VkFormatToRHITextureFormat(VkFormat format);
    u32 GetNumberOfChannelsForFormat(RHI::TextureFormat format, u32& outByteSizePerChannel);

} // namespace CE
