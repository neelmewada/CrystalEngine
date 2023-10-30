#include "VulkanRHIPrivate.h"

#include "VulkanDescriptorSet.h"

namespace CE
{
	VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice* device, VkDescriptorPool pool)
		: device(device), descriptorPool(pool)
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool;
		
		VkDescriptorSetLayoutCreateInfo c;
		
		VkDescriptorSetLayoutBinding bind{};
		bind.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	}
} // namespace CE

