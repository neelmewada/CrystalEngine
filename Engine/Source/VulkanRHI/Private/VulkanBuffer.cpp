#include "VulkanBuffer.h"

namespace CE
{
	static VkBufferUsageFlags VkBufferUsageFlagsFromBufferBindFlags(RHIBufferBindFlags bindFlags)
	{
		VkBufferUsageFlags bufferUsageFlags = 0;
		if (EnumHasFlag(bindFlags, RHIBufferBindFlags::VertexBuffer))
		{
			bufferUsageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		if (EnumHasFlag(bindFlags, RHIBufferBindFlags::IndexBuffer))
		{
			bufferUsageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		if (EnumHasFlag(bindFlags, RHIBufferBindFlags::UniformBuffer))
		{
			bufferUsageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		if (EnumHasFlag(bindFlags, RHIBufferBindFlags::StorageBuffer))
		{
			bufferUsageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		if (EnumHasFlag(bindFlags, RHIBufferBindFlags::StagingBuffer))
		{
			bufferUsageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		}

		return bufferUsageFlags;
	}

	VulkanBuffer::VulkanBuffer(VulkanDevice* device, const RHIBufferDesc& desc)
		: device(device)
		, bindFlags(desc.bindFlags)
		, usageFlags(desc.usageFlags)
		, allocMode(desc.allocMode)
		, name(desc.name)
		, bufferSize(desc.bufferSize)
		, structureByteStride(desc.structureByteStride)
	{
		VkBufferCreateInfo bufferCI{};
		bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCI.size = desc.bufferSize;
		
		bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCI.queueFamilyIndexCount = 0;
		bufferCI.pQueueFamilyIndices = nullptr;

		bufferCI.usage = VkBufferUsageFlagsFromBufferBindFlags(desc.bindFlags) | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		
		if (vkCreateBuffer(device->GetHandle(), &bufferCI, nullptr, &buffer) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create buffer with name {} of size {} bytes", name, bufferSize);
			return;
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device->GetHandle(), buffer, &memRequirements);

		VkMemoryPropertyFlags memoryFlags{};
		if (allocMode == RHIBufferAllocMode::Default || allocMode == RHIBufferAllocMode::SharedMemory)
		{
			memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
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

	VulkanBuffer::~VulkanBuffer()
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
	}

	RHIBufferBindFlags VulkanBuffer::GetBindFlags()
	{
		return bindFlags;
	}

	void VulkanBuffer::UploadData(const RHIBufferData& bufferData)
	{
		if (allocMode == RHIBufferAllocMode::Default || allocMode == RHIBufferAllocMode::SharedMemory)
		{
			// Shared Memory
			void* ptr;
			vkMapMemory(device->GetHandle(), bufferMemory, 0, bufferSize, 0, &ptr);
			memcpy((void*)((SIZE_T)ptr + (SIZE_T)bufferData.startOffsetInBuffer), bufferData.data, bufferData.dataSize);
			vkUnmapMemory(device->GetHandle(), bufferMemory);
		}
		else
		{
			// GPU Memory
			UploadDataToGPU(bufferData);
		}
	}

	void VulkanBuffer::UploadDataToGPU(const RHIBufferData& bufferData)
	{
		if (allocMode != RHIBufferAllocMode::GpuMemory)
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

		RHIBufferData stagingBufferData{};
		stagingBufferData.data = bufferData.data;
		stagingBufferData.dataSize = bufferData.dataSize;
		stagingBufferData.startOffsetInBuffer = 0;

		RHIBufferDesc stagingBufferDesc{};
		stagingBufferDesc.name = "Staging Buffer";
		stagingBufferDesc.bindFlags = RHIBufferBindFlags::StagingBuffer;
		stagingBufferDesc.allocMode = RHIBufferAllocMode::SharedMemory;
		stagingBufferDesc.bufferSize = bufferData.dataSize;
		stagingBufferDesc.usageFlags = RHIBufferUsageFlags::Default;
		stagingBufferDesc.structureByteStride = bufferData.dataSize;
		stagingBufferDesc.initialData = &stagingBufferData;

		auto stagingBuffer = new VulkanBuffer(device, stagingBufferDesc);

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

} // namespace CE
