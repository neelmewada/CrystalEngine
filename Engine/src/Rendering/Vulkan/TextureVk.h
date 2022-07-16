#pragma once

#include "Rendering/ITexture.h"
#include "DeviceContextVk.h"

#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"

#include "TextureViewVk.h"

namespace Vox
{

class DeviceContextVk;

class TextureVk : public ITexture
{
public:
    friend class DeviceContextVk;
    TextureVk(const TextureCreateInfo& createInfo, DeviceContextVk* pDevice);
    ~TextureVk();

    DELETE_COPY_CONSTRUCTORS(TextureVk)

public: // Public API

    ITextureView* GetDefaultView() override { return m_pDefaultView; }

    TextureViewVk* GetDefaultViewInternal() { return m_pDefaultView; }
    Uint32 GetNumOfBytesPerPixel();
    ImageFormat GetImageFormat() { return m_Format; }
    VkImage GetImage() { return m_Image; }

private: // Internal API
    void CreateTexture(const TextureCreateInfo& createInfo);
    void CreateDefaultTextureView(const TextureCreateInfo& createInfo);

    // - Helpers
    void TransitionImageLayout(VkCommandBuffer cmdBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

private: // Internal Members
    DeviceContextVk* m_pDevice = nullptr;
    ImageFormat m_Format;
    TextureType m_TextureType;
    Uint32 m_MipLevels = 1;
    VkImageLayout m_ImageLayout;
    const char* m_pName = nullptr;

    TextureViewVk* m_pDefaultView = nullptr;

private: // Vulkan Members
    VkImage m_Image = nullptr;
    VmaAllocation m_ImageAlloc = nullptr;

};

}
