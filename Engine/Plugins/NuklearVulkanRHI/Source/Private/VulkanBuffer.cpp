#include "VulkanBuffer.h"

namespace CE
{
	static VkBufferUsageFlags VkBufferUsageFlagsFromBufferBindFlags(RHIBufferBindFlags bindFlags)
	{
		VkBufferUsageFlags bufferUsageFlags = 0;
		if ((bindFlags & RHIBufferBindFlags::VertexBuffer) != 0)
			bufferUsageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		if ((bindFlags & RHIBufferBindFlags::IndexBuffer) != 0)
			bufferUsageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		if ((bindFlags & RHIBufferBindFlags::UniformBuffer) != 0)
			bufferUsageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		if ((bindFlags & RHIBufferBindFlags::StorageBuffer) != 0)
			bufferUsageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if ((bindFlags & RHIBufferBindFlags::StagingBuffer) != 0)
			bufferUsageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		//LOG_ERROR_IF(bufferUsageFlags == 0, "ERROR: Invalid buffer bind flag supplied: %i", (int)BindFlag);

		return bufferUsageFlags;
	}

	VulkanBuffer::VulkanBuffer(VulkanDevice* device, const RHIBufferDesc& desc)
		: device(device)
		, bindFlags(desc.bindFlags)
		, usageFlags(desc.usageFlags)
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

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	}

	VulkanBuffer::~VulkanBuffer()
	{

	}

	RHIBufferBindFlags VulkanBuffer::GetBindFlags()
	{
		return bindFlags;
	}

} // namespace CE
