#pragma once

#include "Rendering/ITextureView.h"
#include "DeviceContextVk.h"

#include <vulkan/vulkan.h>

namespace Vox
{

struct TextureViewCreateInfo
{
    VkFormat format;
    VkImage image;
    VkImageLayout imageLayout;
    TextureType textureType;
    Uint32 mipLevels;
};

class TextureViewVk : public ITextureView
{
public:
    TextureViewVk(const TextureViewCreateInfo& createInfo, DeviceContextVk* pDevice);
    ~TextureViewVk();

public: // Public API
    VkImageView GetImageView() { return m_ImageView; }
    VkImageLayout GetImageLayout() { return m_ImageLayout; }
    VkSampler GetSampler() { return m_Sampler; }

private: // Internal API

private: // Members
    DeviceContextVk* m_pDevice = nullptr;
    VkImageView m_ImageView = nullptr;
    VkImageLayout m_ImageLayout;
    VkSampler m_Sampler = nullptr;

};

}

