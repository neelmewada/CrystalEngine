
#include "BufferVk.h"

#include <stdexcept>

using namespace Vox;

BufferVk::BufferVk(const BufferCreateInfo& createInfo, BufferData& bufferData, DeviceContextVk* pDevice)
{
    m_pDevice = pDevice;
    m_VmaAllocator = pDevice->GetVmaAllocator();

    if (createInfo.size != bufferData.dataSize)
    {
        throw std::runtime_error("ERROR: BufferVk constructor passed with 2 different sizes in createInfo & bufferData!");
    }

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = createInfo.size;
    m_BufferSize = bufferInfo.size;
    bufferInfo.usage = VkBufferUsageFlagsFromBufferBindFlags(createInfo.bindFlags);
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Exclusive to 1 queue

    // -- VMA Allocation --
    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = static_cast<VmaMemoryUsage>(createInfo.allocFlags);

    auto result = vmaCreateBuffer(m_VmaAllocator, &bufferInfo,
                                  &allocationInfo, &m_Buffer, &m_Allocation, nullptr);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create VkBuffer using vmaCreateBuffer!");
    }

    if (bufferData.dataSize <= 0 || bufferData.pData == nullptr)
        return;

    // -- Copy Data To Buffer --
    void* data;
    vmaMapMemory(m_VmaAllocator, m_Allocation, &data);
    memcpy(data, bufferData.pData, bufferData.dataSize);
    vmaUnmapMemory(m_VmaAllocator, m_Allocation);
}

BufferVk::~BufferVk()
{
    vmaDestroyBuffer(m_VmaAllocator, m_Buffer, m_Allocation);
}

void BufferVk::SetBufferData(BufferData& bufferData)
{
    if (bufferData.dataSize <= 0 || bufferData.pData == nullptr)
        return;

    // -- Copy Data To Buffer --
    void* data;
    vmaMapMemory(m_VmaAllocator, m_Allocation, &data);
    memcpy(data, bufferData.pData, bufferData.dataSize);
    vmaUnmapMemory(m_VmaAllocator, m_Allocation);
}

VkBufferUsageFlags BufferVk::VkBufferUsageFlagsFromBufferBindFlags(BufferBindFlags bindFlag)
{
    switch (bindFlag)
    {
        case BIND_VERTEX_BUFFER:
            return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        case BIND_INDEX_BUFFER:
            return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        case BIND_UNIFORM_BUFFER:
            return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        case BIND_STORAGE_BUFFER:
            return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }

    throw std::runtime_error("ERROR: Invalid buffer bind flag supplied: " + std::to_string(bindFlag));
}

