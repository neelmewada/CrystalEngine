#pragma once

#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    
    class Buffer : public RHI::Buffer
    {
    public:
		Buffer(VulkanDevice* device, const RHI::BufferDescriptor& desc);
		Buffer(VulkanDevice* device, const RHI::BufferDescriptor& desc, const RHI::ResourceMemoryDescriptor& memoryDesc);
        virtual ~Buffer();

        virtual void* GetHandle() override
        {
            return buffer;
        }

        VkBuffer GetBuffer() { return buffer; }

        virtual void UploadData(const RHI::BufferData& bufferData) override;

        //! Allocates a raw buffer in CPU memory and reads buffer data into it. You are responsible for releasing outData memory block using Memory::Free().
		virtual void ReadData(u8** outData, u64* outDataSize) override;

		virtual void ReadData(void* data) override;

    private:
        void CreateUploadContext();
        
        void UploadDataToGPU(const RHI::BufferData& bufferData);

		void ReadDataFromGPU(u8** outData, u64* outDataSize);
		void ReadDataFromGPU(void* data);
        
        void Free();

    private:

		// External memory heap
		MemoryHeap* memoryHeap = nullptr;
		u64 memoryOffset = 0;

		//! Queue family index of the queue that this buffer is used with the first time
		int initialFamilyIndex = -1;

        VulkanDevice* device = nullptr;
        VkBuffer buffer = nullptr;
		RHI::MemoryHeapType heapType = RHI::MemoryHeapType::Default;
        VkDeviceMemory bufferMemory = nullptr;

        bool uploadContextExists = false;
        VkFence uploadFence = nullptr;
        VkCommandPool uploadCmdPool = nullptr;
        VkCommandBuffer uploadCmdBuffer = nullptr;
    };

} // namespace CE
