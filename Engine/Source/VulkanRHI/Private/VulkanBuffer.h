#pragma once

#include "VulkanRHIPrivate.h"

namespace CE
{
    
    class VulkanBuffer : public RHI::Buffer
    {
    public:
        VulkanBuffer(VulkanDevice* device, const RHI::BufferDesc& desc);
        virtual ~VulkanBuffer();

        virtual RHI::BufferBindFlags GetBindFlags() override;

        virtual void* GetHandle() override
        {
            return buffer;
        }

        VkBuffer GetBuffer() { return buffer; }

        virtual void UploadData(const RHI::BufferData& bufferData) override;

    private:
        void UploadDataToGPU(const RHI::BufferData& bufferData);

    private:
        Name name{};
        RHI::BufferBindFlags bindFlags{};
        RHI::BufferUsageFlags usageFlags{};
        RHI::BufferAllocMode allocMode{};

        u64 bufferSize = 0, structureByteStride = 0;

        VulkanDevice* device = nullptr;
        VkBuffer buffer = nullptr;
        VkDeviceMemory bufferMemory = nullptr;

        bool uploadContextExists = false;
        VkFence uploadFence = nullptr;
        VkCommandPool uploadCmdPool = nullptr;
        VkCommandBuffer uploadCmdBuffer = nullptr;
    };

} // namespace CE
