#pragma once

#include "Rendering/IBuffer.h"
#include "DeviceContextVk.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace Vox
{

class DeviceContextVk;

class BufferVk : public IBuffer
{
private:
    friend class DeviceContextVk;
    BufferVk(const BufferCreateInfo& createInfo, BufferData &bufferData, DeviceContextVk* pDevice);
    ~BufferVk();

public: // Public API
    // - Getters
    VkBuffer GetBuffer() { return m_Buffer; }
    uint64_t GetBufferSize() override { return m_BufferSize; }

private: // Internal API
    VkBufferUsageFlags VkBufferUsageFlagsFromBufferBindFlags(BufferBindFlags bindFlag);

private: // Internal Members
    DeviceContextVk* m_pDevice;

private: // Vulkan Members
    VmaAllocator m_VmaAllocator;
    VmaAllocation m_Allocation;
    VkBuffer m_Buffer;
    uint64_t m_BufferSize;
};

}
