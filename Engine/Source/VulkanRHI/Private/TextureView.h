#pragma once

namespace CE::Vulkan
{

    class TextureView : public RHI::TextureView
    {
    public:

        TextureView(VulkanDevice* device, const RHI::TextureViewDescriptor& desc);

        ~TextureView();

        inline VkImageView GetImageView() const { return imageView; }

    private:

        VulkanDevice* device = nullptr;
        VkImageView imageView = nullptr;
        VkImageAspectFlags aspectMask{};
    };
    
} // namespace CE::Vulkan
