#pragma once

#include "VulkanRHIPrivate.h"

namespace CE
{
    
    class VulkanBuffer : public RHIBuffer
    {
    public:
        VulkanBuffer(VulkanDevice* device, const RHIBufferDesc& desc);
        virtual ~VulkanBuffer();

        virtual RHIBufferBindFlags GetBindFlags() override;

        virtual void* GetHandle() override
        {
            return buffer;
        }

        virtual void UploadData(const RHIBufferData& bufferData) override;

    private:
        void UploadDataToGPU(const RHIBufferData& bufferData);

    private:
        Name name{};
        RHIBufferBindFlags bindFlags{};
        RHIBufferUsageFlags usageFlags{};
        RHIBufferAllocMode allocMode{};

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
