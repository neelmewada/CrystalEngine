
#include "BufferVk.h"

#include <stdexcept>

using namespace Vox;

BufferVk::BufferVk(const BufferCreateInfo& createInfo, BufferData& bufferData, DeviceContextVk* pDevice)
{
    m_pDevice = pDevice;
    m_VmaAllocator = pDevice->GetVmaAllocator();
    m_AllocType = createInfo.allocType;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = createInfo.size;
    m_BufferSize = bufferInfo.size;
    bufferInfo.usage = VkBufferUsageFlagsFromBufferBindFlags(createInfo.bindFlags);
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Exclusive to 1 queue

    // -- VMA Allocation --
    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = static_cast<VmaMemoryUsage>(createInfo.allocType);

    auto result = vmaCreateBuffer(m_VmaAllocator, &bufferInfo,
                                  &allocationInfo, &m_Buffer, &m_Allocation, nullptr);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create VkBuffer using vmaCreateBuffer!");
    }

    // -- Copy Data To Buffer --
    SendBufferData(bufferData);
}

BufferVk::~BufferVk()
{
    vmaDestroyBuffer(m_VmaAllocator, m_Buffer, m_Allocation);
}

void BufferVk::SendBufferData(BufferData& bufferData)
{
    if (bufferData.dataSize <= 0 || bufferData.pData == nullptr)
        return;

    // ONLY if VmaMemoryUsage is set to: CPU_TO_GPU (aka Host & Device visible memory)
    if (m_AllocType == BUFFER_MEM_CPU_TO_GPU)
    {
        void* data;
        vmaMapMemory(m_VmaAllocator, m_Allocation, &data);
        memcpy((void*)(data), bufferData.pData, bufferData.dataSize); // TODO: Add offset to data pointer
        vmaUnmapMemory(m_VmaAllocator, m_Allocation);
    }
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

