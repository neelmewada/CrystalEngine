
#include "TextureViewVk.h"

#include <vulkan/vulkan.h>

using namespace Vox;


TextureViewVk::TextureViewVk(const TextureViewCreateInfo& createInfo, DeviceContextVk* pDevice)
{
    m_pDevice = pDevice;
    m_ImageLayout = createInfo.imageLayout;

    VkImageViewCreateInfo imageViewInfo = {};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.format = createInfo.format;
    imageViewInfo.image = createInfo.image;
    imageViewInfo.viewType = static_cast<VkImageViewType>(createInfo.textureType);

    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = createInfo.mipLevels;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;

    VK_ASSERT(vkCreateImageView(m_pDevice->GetDevice(), &imageViewInfo, nullptr, &m_ImageView),
              "Failed to create image view!");
}

TextureViewVk::~TextureViewVk()
{
    vkDestroyImageView(m_pDevice->GetDevice(), m_ImageView, nullptr);
}



