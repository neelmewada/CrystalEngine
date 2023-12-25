#pragma once

#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    
    class Buffer : public RHI::Buffer
    {
    public:
        Buffer(VulkanDevice* device, const RHI::BufferDesc& desc);
        virtual ~Buffer();

        virtual void* GetHandle() override
        {
            return buffer;
        }

        VkBuffer GetBuffer() { return buffer; }

        virtual void UploadData(const RHI::BufferData& bufferData) override;

        /// Allocates a raw buffer in CPU memory and reads buffer data into it. You are responsible for releasing outData memory block using Memory::Free().
		virtual void ReadData(u8** outData, u64* outDataSize) override;
        
        virtual void Resize(u64 newBufferSize) override;

    private:
        void CreateUploadContext();
        
        void UploadDataToGPU(const RHI::BufferData& bufferData);

		void ReadDataFromGPU(u8** outData, u64* outDataSize);
        
        void Free();

    private:
        Name name{};
        RHI::BufferUsageFlags usageFlags{};
        RHI::BufferAllocMode allocMode{};

        VulkanDevice* device = nullptr;
        VkBuffer buffer = nullptr;
        VkDeviceMemory bufferMemory = nullptr;

        bool uploadContextExists = false;
        VkFence uploadFence = nullptr;
        VkCommandPool uploadCmdPool = nullptr;
        VkCommandBuffer uploadCmdBuffer = nullptr;
    };

} // namespace CE
