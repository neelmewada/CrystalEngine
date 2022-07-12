
#include "BufferVk.h"

#include <stdexcept>
#include <limits>

using namespace Vox;

BufferVk::BufferVk(const BufferCreateInfo& createInfo, DeviceContextVk* pDevice)
{
    m_pDevice = pDevice;
    m_VmaAllocator = pDevice->GetVmaAllocator();
    m_AllocType = createInfo.allocType;
    m_BindFlags = createInfo.bindFlags;
    m_OptimizationFlags = createInfo.optimizationFlags;
    m_pName = createInfo.pName;

    VOX_ASSERT(createInfo.allocType != BUFFER_MEM_UNKNOWN,
               "ERROR: Buffer " + std::string(m_pName) + " created with allocType of " + std::to_string(m_AllocType));

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = createInfo.size;
    m_BufferSize = bufferInfo.size;
    bufferInfo.usage = VkBufferUsageFlagsFromBufferBindFlags(createInfo.bindFlags);
    if (createInfo.allocType == BUFFER_MEM_GPU_ONLY || (createInfo.transferFlags & BUFFER_TRANSFER_DST_BIT))
        bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (createInfo.transferFlags & BUFFER_TRANSFER_SRC_BIT)
        bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Exclusive to 1 queue

    // -- VMA Allocation --
    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = static_cast<VmaMemoryUsage>(createInfo.allocType);

    auto result = vmaCreateBuffer(m_VmaAllocator, &bufferInfo,
                                  &allocationInfo, &m_Buffer, &m_Allocation, nullptr);

    VK_ASSERT(result, "Failed to create VkBuffer using vmaCreateBuffer!");
}

BufferVk::BufferVk(const BufferCreateInfo& createInfo, BufferData& bufferData, DeviceContextVk* pDevice) :
        BufferVk(createInfo, pDevice)
{
    if (createInfo.allocType != BUFFER_MEM_GPU_ONLY)
        SetBufferData(bufferData);
    else
        UploadBufferDataToGPU(bufferData);
}

BufferVk::~BufferVk()
{
    if (m_UploadContextExists)
    {
        m_UploadContextExists = false;
        // Upload CommandPool
        vkDestroyCommandPool(m_pDevice->GetDevice(), m_UploadCmdPool, nullptr);
        // Upload Fence
        vkDestroyFence(m_pDevice->GetDevice(), m_UploadFence, nullptr);
    }

    vmaDestroyBuffer(m_VmaAllocator, m_Buffer, m_Allocation);
}

void BufferVk::SetBufferData(BufferData& bufferData)
{
    if (bufferData.dataSize <= 0 || bufferData.pData == nullptr)
        return;

    // ONLY if VmaMemoryUsage is set to: CPU_TO_GPU or CPU_ONLY
    if (m_AllocType == BUFFER_MEM_CPU_TO_GPU || m_AllocType == BUFFER_MEM_CPU_ONLY)
    {
        void* data;
        vmaMapMemory(m_VmaAllocator, m_Allocation, &data);
        memcpy((void*)(data), bufferData.pData, bufferData.dataSize); // TODO: Add offset to data pointer
        vmaUnmapMemory(m_VmaAllocator, m_Allocation);
    }
}

void BufferVk::UploadBufferDataToGPU(BufferData& bufferData)
{
    if (bufferData.dataSize <= 0 || bufferData.pData == nullptr)
        return;

    if (m_AllocType != BUFFER_MEM_GPU_ONLY)
    {
        throw std::runtime_error("UploadBufferDataToGPU called on Buffer that isn't a GPU_ONLY buffer!");
    }

    bool updatedRegularly = (m_OptimizationFlags & BUFFER_OPTIMIZE_UPDATE_REGULAR_BIT);
    bool seldomUpdated = (m_OptimizationFlags & BUFFER_OPTIMIZE_UPDATE_SELDOM_BIT);
    bool neverUpdated = (m_OptimizationFlags & BUFFER_OPTIMIZE_UPDATE_NEVER_BIT);

    if (!m_UploadContextExists)
    {
        m_UploadContextExists = true;

        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        VK_ASSERT(vkCreateFence(m_pDevice->GetDevice(), &fenceCreateInfo, nullptr, &m_UploadFence),
                  "Failed to Create Upload VkFence for a GPU-ONLY Buffer transfer!");

        VkCommandPoolCreateInfo cmdPoolInfo = {};
        cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolInfo.queueFamilyIndex = m_pDevice->GetQueueFamilies().graphicsFamilyIndex;

        VK_ASSERT(vkCreateCommandPool(m_pDevice->GetDevice(), &cmdPoolInfo, nullptr, &m_UploadCmdPool),
                  "Failed to create Upload CommandPool for a GPU-ONLY Buffer transfer!");

        VkCommandBufferAllocateInfo cmdBufferInfo = {};
        cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdBufferInfo.commandBufferCount = 1;
        cmdBufferInfo.commandPool = m_UploadCmdPool;

        VK_ASSERT(vkAllocateCommandBuffers(m_pDevice->GetDevice(), &cmdBufferInfo, &m_UploadCmdBuffer),
                  "Failed to allocate Upload CommandBuffer for a GPU-ONLY Buffer transfer!");
    }

    BufferCreateInfo stagingBufferInfo = {};
    stagingBufferInfo.size = m_BufferSize;
    stagingBufferInfo.bindFlags = BIND_STAGING_BUFFER;
    stagingBufferInfo.pName = "Staging Buffer";
    stagingBufferInfo.allocType = BUFFER_MEM_CPU_ONLY;
    stagingBufferInfo.optimizationFlags = BUFFER_OPTIMIZE_TEMPORARY_BIT;

    BufferVk* stagingBuffer = new BufferVk(stagingBufferInfo, bufferData, m_pDevice);

    auto uint64_max = std::numeric_limits<uint64_t>::max();

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_ASSERT(vkBeginCommandBuffer(m_UploadCmdBuffer, &beginInfo),
              "Failed to Begin Upload CommandBuffer for a GPU Buffer transfer!");

    VkBufferCopy bufferCopy = {};
    bufferCopy.size = m_BufferSize;
    bufferCopy.srcOffset = 0;
    bufferCopy.dstOffset = 0;
    vkCmdCopyBuffer(m_UploadCmdBuffer, stagingBuffer->GetBuffer(), this->GetBuffer(), 1, &bufferCopy);

    VK_ASSERT(vkEndCommandBuffer(m_UploadCmdBuffer),
              "Failed to End Upload CommandBuffer for a GPU Buffer transfer!");

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_UploadCmdBuffer;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    VK_ASSERT(vkQueueSubmit(m_pDevice->GetGraphicsQueue(), 1, &submitInfo, m_UploadFence),
              "Failed to submit Buffer Copy Command to the main Graphics Queue!");

    // Wait for transfer to finish
    vkWaitForFences(m_pDevice->GetDevice(), 1, &m_UploadFence, VK_TRUE, uint64_max);
    vkResetFences(m_pDevice->GetDevice(), 1, &m_UploadFence);

    // Reset the command buffers in this pool
    vkResetCommandPool(m_pDevice->GetDevice(), m_UploadCmdPool, 0);

    // Release the staging buffer
    delete stagingBuffer;
    stagingBuffer = nullptr;

    if (!updatedRegularly) // Destroy the resources if they aren't updated regularly
    {
        m_UploadContextExists = false;
        // Upload CommandPool
        vkDestroyCommandPool(m_pDevice->GetDevice(), m_UploadCmdPool, nullptr);
        m_UploadCmdPool = nullptr;
        // Upload Fence
        vkDestroyFence(m_pDevice->GetDevice(), m_UploadFence, nullptr);
        m_UploadFence = nullptr;
    }
}

VkBufferUsageFlags BufferVk::VkBufferUsageFlagsFromBufferBindFlags(BufferBindFlags bindFlag)
{
    VkBufferUsageFlags bufferUsageFlags = 0;
    if (bindFlag & BIND_VERTEX_BUFFER)
        bufferUsageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if (bindFlag & BIND_INDEX_BUFFER)
        bufferUsageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (bindFlag & BIND_UNIFORM_BUFFER)
        bufferUsageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if (bindFlag & BIND_STORAGE_BUFFER)
        bufferUsageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if (bindFlag & BIND_STAGING_BUFFER)
        bufferUsageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VOX_ASSERT(bufferUsageFlags != 0, "ERROR: Invalid buffer bind flag supplied: " + std::to_string(bindFlag));

    return bufferUsageFlags;
}

