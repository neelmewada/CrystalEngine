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
public:
    friend class DeviceContextVk;
    BufferVk(const BufferCreateInfo& createInfo, DeviceContextVk* pDevice);
    BufferVk(const BufferCreateInfo& createInfo, BufferData &bufferData, DeviceContextVk* pDevice);
    ~BufferVk();

public:
    DELETE_COPY_CONSTRUCTORS(BufferVk)

public: // Public API
    Type GetDeviceObjectType() override { return DEVICE_OBJECT_BUFFER; }

    // - Getters
    VkBuffer GetBuffer() { return m_Buffer; }
    uint64_t GetBufferSize() override { return m_BufferSize; }

    // - Buffer API
    void SetBufferData(BufferData& bufferData) override;
    void UploadBufferDataToGPU(BufferData &bufferData) override;

private: // Internal API
    VkBufferUsageFlags VkBufferUsageFlagsFromBufferBindFlags(BufferBindFlags bindFlag);

private: // Internal Members
    DeviceContextVk* m_pDevice = nullptr;
    BufferAllocationType m_AllocType;
    BufferBindFlags m_BindFlags;
    BufferOptimizationFlags m_OptimizationFlags;
    const char* m_pName = nullptr;

private: // Vulkan Members
    VkBuffer m_Buffer = nullptr;
    uint64_t m_BufferSize;
    VmaAllocator m_VmaAllocator = nullptr;
    VmaAllocation m_Allocation = nullptr;

    bool m_UploadContextExists = false;
    VkFence m_UploadFence = nullptr;
    VkCommandPool m_UploadCmdPool = nullptr;
    VkCommandBuffer m_UploadCmdBuffer = nullptr;
};

}
