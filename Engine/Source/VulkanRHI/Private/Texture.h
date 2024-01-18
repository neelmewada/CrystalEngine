#pragma once

namespace CE::Vulkan
{
	class VulkanDevice;

    class Texture : public RHI::Texture
    {
    public:
        Texture(VulkanDevice* device, const RHI::TextureDescriptor& desc);
		Texture(VulkanDevice* device, const RHI::TextureDescriptor& desc, const RHI::ResourceMemoryDescriptor& memoryDesc);

		Texture(VulkanDevice* device, VkImage importedImage, const RHI::TextureDescriptor& imageDesc, 
			VkImageLayout dstLayout = VK_IMAGE_LAYOUT_UNDEFINED);

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

		virtual void UploadData(RHI::Buffer* srcBuffer, u64 offsetInBuffer = 0, u32 mipLevel = 0, u32 arrayLayer = 0);

		inline VkImageLayout GetVkImageLayout() const { return vkImageLayout; }

    protected:

        void CopyPixelsFromBuffer(Buffer* srcBuffer);

		void CopyPixelsToBuffer(Buffer* dstBuffer);

    private:

		void Init(const RHI::TextureDescriptor& desc);
		void AllocateInternal();
		void PostInit(const RHI::TextureDescriptor& desc);
		
		// True if the image is not managed by this Texture object
		bool importedImage = false;

		int curFamilyIndex = -1;

        VulkanDevice* device = nullptr;
        VkImage image = nullptr;
        VkDeviceMemory imageMemory = nullptr;
        VkImageView imageView = nullptr;

		MemoryHeapType heapType{};
        
        VkFormat vkFormat{};
		VkImageLayout vkImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageAspectFlags aspectMask{};

		friend class RHI::FrameGraph;
		friend class GraphicsCommandList;
		friend class FrameGraphExecuter;
    };
    
    VkFormat RHIFormatToVkFormat(RHI::TextureFormat format);
    RHI::TextureFormat VkFormatToRHIFormat(VkFormat format);
    u32 GetNumberOfChannelsForFormat(RHI::TextureFormat format, u32& outByteSizePerChannel);

	bool IsDepthVkFormat(VkFormat format);
	bool IsStencilVkFormat(VkFormat format);

	bool IsDepthFormat(RHI::Format format);
	bool IsDepthStencilFormat(RHI::Format format);

} // namespace CE
