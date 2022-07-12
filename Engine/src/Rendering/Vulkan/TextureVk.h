#pragma once

#include "Rendering/ITexture.h"
#include "DeviceContextVk.h"

#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"

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
    Uint32 GetNumOfBytesPerPixel();
    ImageFormat GetImageFormat() { return m_Format; }

private: // Internal API
    void CreateTexture(const TextureCreateInfo& createInfo);

    // - Helpers
    void TransitionImageLayout(VkCommandBuffer cmdBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

private: // Internal Members
    DeviceContextVk* m_pDevice = nullptr;
    ImageFormat m_Format;
    int m_MipLevels = 1;
    const char* m_pName = nullptr;

private: // Vulkan Members
    VkImage m_Image = nullptr;
    VmaAllocation m_ImageAlloc = nullptr;

};

}
