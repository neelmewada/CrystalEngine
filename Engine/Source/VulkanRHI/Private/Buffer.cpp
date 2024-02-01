
#include "VulkanRHI.h"
#include "VulkanRHIPrivate.h"

#include "Buffer.h"

namespace CE::Vulkan
{
	static VkBufferUsageFlags VkBufferUsageFlagsFromBufferBindFlags(RHI::BufferBindFlags bindFlags)
	{
		VkBufferUsageFlags bufferUsageFlags = 0;
		if (EnumHasFlag(bindFlags, RHI::BufferBindFlags::VertexBuffer))
		{
			bufferUsageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		if (EnumHasFlag(bindFlags, RHI::BufferBindFlags::IndexBuffer))
		{
			bufferUsageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		if (EnumHasFlag(bindFlags, RHI::BufferBindFlags::ConstantBuffer))
		{
			bufferUsageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		if (EnumHasFlag(bindFlags, RHI::BufferBindFlags::StructuredBuffer))
		{
			bufferUsageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		if (EnumHasFlag(bindFlags, RHI::BufferBindFlags::StagingBuffer))
		{
			bufferUsageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}
		
		return bufferUsageFlags;
	}

	void VulkanRHI::GetBufferMemoryRequirements(const BufferDescriptor& bufferDesc, ResourceMemoryRequirements& outRequirements)
	{
		VkBuffer tempBuffer = nullptr;
		VkBufferCreateInfo tempBufferCI{};
		tempBufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		tempBufferCI.size = bufferDesc.bufferSize;

		tempBufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		tempBufferCI.queueFamilyIndexCount = 0;
		tempBufferCI.pQueueFamilyIndices = nullptr;

		tempBufferCI.usage = VkBufferUsageFlagsFromBufferBindFlags(bufferDesc.bindFlags) | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		if (vkCreateBuffer(device->GetHandle(), &tempBufferCI, nullptr, &tempBuffer) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create buffer with name {} of size {} bytes", bufferDesc.name, bufferDesc.bufferSize);
			return;
		}

		VkMemoryRequirements bufferRequirements{};
		vkGetBufferMemoryRequirements(device->GetHandle(), tempBuffer, &bufferRequirements);
		outRequirements.size = bufferRequirements.size;
		outRequirements.offsetAlignment = bufferRequirements.alignment;
		outRequirements.flags = bufferRequirements.memoryTypeBits;
		
		vkDestroyBuffer(device->GetHandle(), tempBuffer, nullptr);
	}

	Buffer::Buffer(VulkanDevice* device, const RHI::BufferDescriptor& desc)
		: device(device)
	{
		name = desc.name;
		bindFlags = desc.bindFlags;
		bufferSize = desc.bufferSize;
		heapType = desc.defaultHeapType;
		structureByteStride = desc.structureByteStride;

		VkBufferCreateInfo bufferCI{};
		bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCI.size = desc.bufferSize;
		
		bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCI.queueFamilyIndexCount = 0;
		bufferCI.pQueueFamilyIndices = nullptr;
		
		bufferCI.usage = VkBufferUsageFlagsFromBufferBindFlags(desc.bindFlags) | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		
		if (vkCreateBuffer(device->GetHandle(), &bufferCI, nullptr, &buffer) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create buffer with name {} of size {} bytes", name, bufferSize);
			return;
		}
		
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device->GetHandle(), buffer, &memRequirements);

		VkMemoryPropertyFlags memoryFlags{};
		if (device->IsUnifiedMemoryArchitecture() || heapType == RHI::MemoryHeapType::Upload || heapType == RHI::MemoryHeapType::ReadBack)
		{
			memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			if (heapType == RHI::MemoryHeapType::ReadBack)
				memoryFlags |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
			if (device->IsUnifiedMemoryArchitecture())
				memoryFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}
		else
		{
			memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits, memoryFlags);
		allocInfo.pNext = nullptr;
		
		if (vkAllocateMemory(device->GetHandle(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to allocate memory to buffer with name {} of size {} bytes", name, bufferSize);
			return;
		}

		vkBindBufferMemory(device->GetHandle(), buffer, bufferMemory, 0);
	}

	Buffer::Buffer(VulkanDevice* device, const RHI::BufferDescriptor& desc, const RHI::ResourceMemoryDescriptor& memoryDesc)
		: device(device)
		, memoryHeap((Vulkan::MemoryHeap*)memoryDesc.memoryHeap)
		, memoryOffset(memoryDesc.memoryOffset)
	{
		name = desc.name;
		bindFlags = desc.bindFlags;
		bufferSize = desc.bufferSize;
		heapType = memoryHeap->GetHeapType();
		structureByteStride = desc.structureByteStride;
        bufferMemory = nullptr;

		VkBufferCreateInfo bufferCI{};
		bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCI.size = desc.bufferSize;

		bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCI.queueFamilyIndexCount = 0;
		bufferCI.pQueueFamilyIndices = nullptr;
		
		bufferCI.usage = VkBufferUsageFlagsFromBufferBindFlags(desc.bindFlags) | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		if (vkCreateBuffer(device->GetHandle(), &bufferCI, nullptr, &buffer) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create buffer with name {} of size {} bytes", name, bufferSize);
			return;
		}
        
		bool success = memoryHeap->BindBuffer(this, memoryOffset);
		if (!success)
		{
			return;
		}
	}

	Buffer::~Buffer()
	{
        Free();
	}

    void Buffer::Free()
    {
        if (buffer != nullptr)
        {
            vkDestroyBuffer(device->GetHandle(), buffer, nullptr);
            buffer = nullptr;
        }

        if (bufferMemory != nullptr)
        {
            vkFreeMemory(device->GetHandle(), bufferMemory, nullptr);
            bufferMemory = nullptr;
        }
        
        if (uploadCmdPool)
        {
            vkDestroyCommandPool(device->GetHandle(), uploadCmdPool, nullptr);
            uploadCmdPool = nullptr;
        }
        
        if (uploadFence)
        {
            vkDestroyFence(device->GetHandle(), uploadFence, nullptr);
            uploadFence = nullptr;
        }
        
        uploadCmdBuffer = nullptr;
        
        uploadContextExists = false;
    }

	void Buffer::UploadData(const RHI::BufferData& bufferData)
	{
		if (device->IsUnifiedMemoryArchitecture() || heapType == RHI::MemoryHeapType::Upload || heapType == RHI::MemoryHeapType::ReadBack)
		{
			if (bufferMemory != nullptr) // Self allocated memory
			{
				void* ptr;
				vkMapMemory(device->GetHandle(), bufferMemory, 0, bufferSize, 0, &ptr);
				memcpy((void*)((SIZE_T)ptr + (SIZE_T)bufferData.startOffsetInBuffer), bufferData.data, bufferData.dataSize);
				vkUnmapMemory(device->GetHandle(), bufferMemory);
			}
			else if (memoryHeap != nullptr) // Externally managed memory
			{
				void* ptr;
				vkMapMemory(device->GetHandle(), memoryHeap->GetHandle(), memoryOffset, bufferSize, 0, &ptr);
				memcpy((void*)((SIZE_T)ptr + (SIZE_T)bufferData.startOffsetInBuffer), bufferData.data, bufferData.dataSize);
				vkUnmapMemory(device->GetHandle(), memoryHeap->GetHandle());
			}
		}
		else
		{
			// GPU exclusive Memory
			UploadDataToGPU(bufferData);
		}
	}

	void Buffer::ReadData(u8** outData, u64* outDataSize)
	{
		if (outData == nullptr || outDataSize == nullptr)
			return;

		*outData = nullptr;
		*outDataSize = 0;

		if (device->IsUnifiedMemoryArchitecture() || heapType == RHI::MemoryHeapType::Upload || heapType == RHI::MemoryHeapType::ReadBack)
		{
			// Shared memory
			*outDataSize = bufferSize;
			*outData = (u8*)Memory::Malloc(bufferSize);
			if (bufferMemory != nullptr) // Self allocated memory
			{
				void* ptr = nullptr;
				vkMapMemory(device->GetHandle(), bufferMemory, 0, bufferSize, 0, &ptr);
				memcpy(*outData, ptr, bufferSize);
				vkUnmapMemory(device->GetHandle(), bufferMemory);
			}
			else if (memoryHeap != nullptr) // Externally managed memory
			{
				void* ptr = nullptr;
				vkMapMemory(device->GetHandle(), memoryHeap->GetHandle(), memoryOffset, bufferSize, 0, &ptr);
				memcpy(*outData, ptr, bufferSize);
				vkUnmapMemory(device->GetHandle(), memoryHeap->GetHandle());
			}
		}
		else
		{
			// GPU Memory
			ReadDataFromGPU(outData, outDataSize);
		}
	}

	void Buffer::ReadData(void* data)
	{
		if (device->IsUnifiedMemoryArchitecture() || heapType == RHI::MemoryHeapType::Upload || heapType == RHI::MemoryHeapType::ReadBack)
		{
			if (bufferMemory != nullptr) // Self allocated memory
			{
				void* ptr = nullptr;
				vkMapMemory(device->GetHandle(), bufferMemory, 0, bufferSize, 0, &ptr);
				memcpy(data, ptr, bufferSize);
				vkUnmapMemory(device->GetHandle(), bufferMemory);
			}
			else if (memoryHeap != nullptr) // Externally managed memory
			{
				void* ptr = nullptr;
				vkMapMemory(device->GetHandle(), memoryHeap->GetHandle(), memoryOffset, bufferSize, 0, &ptr);
				memcpy(data, ptr, bufferSize);
				vkUnmapMemory(device->GetHandle(), memoryHeap->GetHandle());
			}
		}
		else
		{
			// GPU Memory
			ReadDataFromGPU(data);
		}
	}

	bool Buffer::Map(u64 offset, u64 size, void** outPtr)
	{
		if (isMapped)
			return false;

		if (device->IsUnifiedMemoryArchitecture() || heapType == RHI::MemoryHeapType::Upload || heapType == RHI::MemoryHeapType::ReadBack)
		{
			if (bufferMemory != nullptr) // Self allocated memory
			{
				isMapped = true;
				vkMapMemory(device->GetHandle(), bufferMemory, offset, size, 0, outPtr);
				return true;
			}
			else if (memoryHeap != nullptr) // Externally managed memory
			{
				isMapped = true;
				vkMapMemory(device->GetHandle(), memoryHeap->GetHandle(), memoryOffset + offset, size, 0, outPtr);
				return true;
			}
		}

		return false;
	}

	bool Buffer::Unmap()
	{
		if (!isMapped)
			return false;

		if (device->IsUnifiedMemoryArchitecture() || heapType == RHI::MemoryHeapType::Upload || heapType == RHI::MemoryHeapType::ReadBack)
		{
			if (bufferMemory != nullptr) // Self allocated memory
			{
				isMapped = false;
				vkUnmapMemory(device->GetHandle(), bufferMemory);
				return true;
			}
			else if (memoryHeap != nullptr) // Externally managed memory
			{
				isMapped = false;
				vkUnmapMemory(device->GetHandle(), memoryHeap->GetHandle());
				return true;
			}
		}

		return false;
	}

    void Buffer::CreateUploadContext()
    {
        if (!uploadContextExists)
        {
            VkFenceCreateInfo fenceCI{};
            fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            
            if (vkCreateFence(device->GetHandle(), &fenceCI, nullptr, &uploadFence) != VK_SUCCESS)
            {
                CE_LOG(Error, All, "Failed to create GPU Buffer Upload Fence for buffer {}", name);
                return;
            }

            VkCommandPoolCreateInfo cmdPoolCI{};
            cmdPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            cmdPoolCI.queueFamilyIndex = device->GetGraphicsQueue()->GetFamilyIndex();
            
            if (vkCreateCommandPool(device->GetHandle(), &cmdPoolCI, nullptr, &uploadCmdPool) != VK_SUCCESS)
            {
                vkDestroyFence(device->GetHandle(), uploadFence, nullptr);
                uploadFence = nullptr;
                CE_LOG(Error, All, "Failed to create GPU Buffer Upload Command Pool for buffer {}", name);
                return;
            }

            uploadContextExists = true;
        }
    }

	void Buffer::UploadDataToGPU(const RHI::BufferData& bufferData)
	{
		if (heapType != RHI::MemoryHeapType::Default)
			return;

		if (!uploadContextExists)
		{
			VkFenceCreateInfo fenceCI{};
			fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			
			if (vkCreateFence(device->GetHandle(), &fenceCI, nullptr, &uploadFence) != VK_SUCCESS)
			{
				CE_LOG(Error, All, "Failed to create GPU Buffer Upload Fence for buffer {}", name);
				return;
			}

			VkCommandPoolCreateInfo cmdPoolCI{};
			cmdPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmdPoolCI.queueFamilyIndex = device->GetGraphicsQueue()->GetFamilyIndex();
			
			if (vkCreateCommandPool(device->GetHandle(), &cmdPoolCI, nullptr, &uploadCmdPool) != VK_SUCCESS)
			{
				vkDestroyFence(device->GetHandle(), uploadFence, nullptr);
				uploadFence = nullptr;
				CE_LOG(Error, All, "Failed to create GPU Buffer Upload Command Pool for buffer {}", name);
				return;
			}

			uploadContextExists = true;
		}

		if (!uploadContextExists)
		{
			CE_LOG(Error, All, "Failed to upload data to GPU buffer! Could not create upload context for buffer {}", name);
			return;
		}
		
		VkCommandBufferAllocateInfo cmdBufferInfo{};
		cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufferInfo.commandBufferCount = 1;
		cmdBufferInfo.commandPool = uploadCmdPool;

		if (vkAllocateCommandBuffers(device->GetHandle(), &cmdBufferInfo, &uploadCmdBuffer) != VK_SUCCESS)
		{
			return;
		}

		RHI::BufferData stagingBufferData{};
		stagingBufferData.data = bufferData.data;
		stagingBufferData.dataSize = bufferData.dataSize;
		stagingBufferData.startOffsetInBuffer = 0;

		RHI::BufferDescriptor stagingBufferDesc{};
		stagingBufferDesc.name = "Staging Buffer";
		stagingBufferDesc.bindFlags = RHI::BufferBindFlags::StagingBuffer;
		stagingBufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
		stagingBufferDesc.bufferSize = bufferData.dataSize;
		stagingBufferDesc.structureByteStride = bufferData.dataSize;

		Buffer* stagingBuffer = new Buffer(device, stagingBufferDesc);

		stagingBuffer->UploadData(stagingBufferData);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(uploadCmdBuffer, &beginInfo);
		{
			VkBufferCopy copy{};
			copy.srcOffset = 0;
			copy.size = bufferData.dataSize;
			copy.dstOffset = bufferData.startOffsetInBuffer;

			vkCmdCopyBuffer(uploadCmdBuffer, stagingBuffer->buffer, this->buffer, 1, &copy);
		}
		vkEndCommandBuffer(uploadCmdBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &uploadCmdBuffer;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		vkQueueSubmit(device->GetGraphicsQueue()->GetHandle(), 1, &submitInfo, uploadFence);

		constexpr auto u64Max = std::numeric_limits<u64>::max();
		vkWaitForFences(device->GetHandle(), 1, &uploadFence, VK_TRUE, u64Max);
		vkResetFences(device->GetHandle(), 1, &uploadFence);

		vkResetCommandPool(device->GetHandle(), uploadCmdPool, 0);

		delete stagingBuffer;
		stagingBuffer = nullptr;
	}

	void Buffer::ReadDataFromGPU(u8** outData, u64* outDataSize)
	{
		if (outData == nullptr || outDataSize == nullptr)
			return;

		*outData = nullptr;
		*outDataSize = 0;

		if (heapType != RHI::MemoryHeapType::Default)
			return;

        if (!uploadContextExists)
        {
            VkFenceCreateInfo fenceCI{};
            fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            
            if (vkCreateFence(device->GetHandle(), &fenceCI, nullptr, &uploadFence) != VK_SUCCESS)
            {
                CE_LOG(Error, All, "Failed to create GPU Buffer Upload Fence for buffer {}", name);
                return;
            }

            VkCommandPoolCreateInfo cmdPoolCI{};
            cmdPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            cmdPoolCI.queueFamilyIndex = device->GetGraphicsQueue()->GetFamilyIndex();
            
            if (vkCreateCommandPool(device->GetHandle(), &cmdPoolCI, nullptr, &uploadCmdPool) != VK_SUCCESS)
            {
                vkDestroyFence(device->GetHandle(), uploadFence, nullptr);
                uploadFence = nullptr;
                CE_LOG(Error, All, "Failed to create GPU Buffer Upload Command Pool for buffer {}", name);
                return;
            }
            
            uploadContextExists = true;
        }
        
        if (!uploadContextExists)
        {
            CE_LOG(Error, All, "Failed to read data from GPU buffer! Could not create upload context for buffer {}", name);
            return;
        }
        
        RHI::BufferDescriptor stagingBufferDesc{};
        stagingBufferDesc.name = "Staging Buffer";
        stagingBufferDesc.bindFlags = RHI::BufferBindFlags::StagingBuffer;
        stagingBufferDesc.defaultHeapType = RHI::MemoryHeapType::ReadBack;
        stagingBufferDesc.bufferSize = bufferSize;
        stagingBufferDesc.structureByteStride = structureByteStride;
        
        Buffer* stagingBuffer = new Buffer(device, stagingBufferDesc);
        
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
        vkBeginCommandBuffer(uploadCmdBuffer, &beginInfo);
        {
            VkBufferCopy copy{};
            copy.srcOffset = 0;
            copy.size = bufferSize;
            copy.dstOffset = 0;
            
            vkCmdCopyBuffer(uploadCmdBuffer, this->buffer, stagingBuffer->GetBuffer(), 1, &copy);
        }
        vkEndCommandBuffer(uploadCmdBuffer);
        
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &uploadCmdBuffer;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitDstStageMask = nullptr;
        submitInfo.pWaitSemaphores = nullptr;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;
        
        vkQueueSubmit(device->GetGraphicsQueue()->GetHandle(), 1, &submitInfo, uploadFence);
        
        constexpr u64 u64Max = std::numeric_limits<u64>::max();
        vkWaitForFences(device->GetHandle(), 1, &uploadFence, VK_TRUE, u64Max);
        vkResetFences(device->GetHandle(), 1, &uploadFence);

        vkResetCommandPool(device->GetHandle(), uploadCmdPool, 0);
        
        stagingBuffer->ReadData(outData, outDataSize);

        delete stagingBuffer;
        stagingBuffer = nullptr;
	}

	void Buffer::ReadDataFromGPU(void* data)
	{
		if (heapType != RHI::MemoryHeapType::Default)
			return;

		if (!uploadContextExists)
		{
			VkFenceCreateInfo fenceCI{};
			fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

			if (vkCreateFence(device->GetHandle(), &fenceCI, nullptr, &uploadFence) != VK_SUCCESS)
			{
				CE_LOG(Error, All, "Failed to create GPU Buffer Upload Fence for buffer {}", name);
				return;
			}

			VkCommandPoolCreateInfo cmdPoolCI{};
			cmdPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmdPoolCI.queueFamilyIndex = device->GetGraphicsQueue()->GetFamilyIndex();

			if (vkCreateCommandPool(device->GetHandle(), &cmdPoolCI, nullptr, &uploadCmdPool) != VK_SUCCESS)
			{
				vkDestroyFence(device->GetHandle(), uploadFence, nullptr);
				uploadFence = nullptr;
				CE_LOG(Error, All, "Failed to create GPU Buffer Upload Command Pool for buffer {}", name);
				return;
			}

			uploadContextExists = true;
		}

		if (!uploadContextExists)
		{
			CE_LOG(Error, All, "Failed to read data from GPU buffer! Could not create upload context for buffer {}", name);
			return;
		}

		RHI::BufferDescriptor stagingBufferDesc{};
		stagingBufferDesc.name = "Staging Buffer";
		stagingBufferDesc.bindFlags = RHI::BufferBindFlags::StagingBuffer;
		stagingBufferDesc.defaultHeapType = RHI::MemoryHeapType::ReadBack;
		stagingBufferDesc.bufferSize = bufferSize;
		stagingBufferDesc.structureByteStride = structureByteStride;

		Buffer* stagingBuffer = new Buffer(device, stagingBufferDesc);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(uploadCmdBuffer, &beginInfo);
		{
			VkBufferCopy copy{};
			copy.srcOffset = 0;
			copy.size = bufferSize;
			copy.dstOffset = 0;

			vkCmdCopyBuffer(uploadCmdBuffer, this->buffer, stagingBuffer->GetBuffer(), 1, &copy);
		}
		vkEndCommandBuffer(uploadCmdBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &uploadCmdBuffer;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		vkQueueSubmit(device->GetGraphicsQueue()->GetHandle(), 1, &submitInfo, uploadFence);

		constexpr u64 u64Max = std::numeric_limits<u64>::max();
		vkWaitForFences(device->GetHandle(), 1, &uploadFence, VK_TRUE, u64Max);
		vkResetFences(device->GetHandle(), 1, &uploadFence);

		vkResetCommandPool(device->GetHandle(), uploadCmdPool, 0);

		stagingBuffer->ReadData(data);

		delete stagingBuffer;
		stagingBuffer = nullptr;
	}

} // namespace CE
