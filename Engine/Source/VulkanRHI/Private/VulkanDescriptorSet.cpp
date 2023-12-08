#include "VulkanRHIPrivate.h"

#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"
#include "VulkanShaderResourceGroup.h"

namespace CE
{

	VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice* device)
		: device(device)
	{

	}

	VulkanDescriptorSet::~VulkanDescriptorSet()
	{
		Destroy();
	}

	void VulkanDescriptorSet::Destroy()
	{
		if (descriptorPool != nullptr)
			vkFreeDescriptorSets(device->GetHandle(), descriptorPool, 1, &descriptorSet);
		descriptorSet = nullptr;

		if (setLayout != nullptr)
			vkDestroyDescriptorSetLayout(device->GetHandle(), setLayout, nullptr);
		setLayout = nullptr;


	}

	int VulkanDescriptorSet::Create(const Array<VulkanShaderResourceGroup*>& srgs)
	{
		VkDescriptorSetLayoutCreateInfo setLayoutCI{};
		setLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

		variableNames.Clear();
		setBindings.Clear();
		variableNameToBinding.Clear();
		bindingSlotToBinding.Clear();

		for (VulkanShaderResourceGroup* srg : srgs)
		{
			variableNames.AddRange(srg->variableNames);
			setBindings.AddRange(srg->setBindings);
			
			variableNameToBinding.AddRange(srg->variableNameToBinding);
			bindingSlotToBinding.AddRange(srg->bindingSlotToBinding);

			bufferVariablesBoundByBindingSlot.AddRange(srg->bufferVariablesBoundByBindingSlot);
			imageVariablesBoundByBindingSlot.AddRange(srg->imageVariablesBoundByBindingSlot);
		}

		VkDescriptorSetLayoutCreateInfo setLayoutCI{};
		setLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		setLayoutCI.bindingCount = setBindings.GetSize();
		setLayoutCI.pBindings = setBindings.GetData();
		
		auto result = vkCreateDescriptorSetLayout(device->GetHandle(), &setLayoutCI, nullptr, &setLayout);
		if (result != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan Descriptor Set Layout. Error {}", (int)result);
			return (int)result;
		}
		
		auto pool = device->GetDescriptorPool();
		auto allocatedSets = pool->Allocate(1, { setLayout }, descriptorPool);

		if (allocatedSets.IsEmpty())
		{
			CE_LOG(Error, All, "Failed to allocate Vulkan Descriptor Set.");
			return -1;
		}

		BindAllVariables();
	}

	void VulkanDescriptorSet::BindAllVariables()
	{
		if (bufferVariablesBoundByBindingSlot.IsEmpty() && 
			imageVariablesBoundByBindingSlot.IsEmpty())
			return;

		Array<VkWriteDescriptorSet> writes{};
		writes.Resize(bufferVariablesBoundByBindingSlot.GetSize() + imageVariablesBoundByBindingSlot.GetSize());
		int count = 0;

		for (const auto& [bindingSlot, bufferInfo] : bufferVariablesBoundByBindingSlot)
		{
			auto buffer = bufferInfo.buffer;
			if (buffer == nullptr)
				continue;
			if (bindingSlot < 0 || !bindingSlotToBinding.KeyExists(bindingSlot))
				continue;

			VkDescriptorSetLayoutBinding bindingInfo = bindingSlotToBinding[bindingSlot];

			writes[count] = {};
			writes[count].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writes[count].dstSet = descriptorSet;
			writes[count].dstBinding = bindingSlot;
			writes[count].dstArrayElement = 0;

			writes[count].descriptorCount = bindingInfo.descriptorCount;
			writes[count].descriptorType = bindingInfo.descriptorType;
			writes[count].pBufferInfo = &bufferInfo;

			count++;
		}

		for (const auto& [bindingSlot, imageInfo] : imageVariablesBoundByBindingSlot)
		{
			if (bindingSlot < 0 || !bindingSlotToBinding.KeyExists(bindingSlot))
				continue;

			VkDescriptorSetLayoutBinding bindingInfo = bindingSlotToBinding[bindingSlot];

			writes[count] = {};
			writes[count].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writes[count].dstSet = descriptorSet;
			writes[count].dstBinding = bindingSlot;
			writes[count].dstArrayElement = 0;

			writes[count].descriptorCount = bindingInfo.descriptorCount;
			writes[count].descriptorType = bindingInfo.descriptorType;
			writes[count].pImageInfo = &imageInfo;

			count++;
		}

		vkUpdateDescriptorSets(device->GetHandle(), count, writes.GetData(), 0, nullptr);
	}

} // namespace CE

