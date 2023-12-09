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

	int VulkanDescriptorSet::Create(Array<VulkanShaderResourceGroup*> srgs)
	{
		variableNames.Clear();
		setBindings.Clear();
		variableNameToBinding.Clear();
		bindingSlotToBinding.Clear();
		sharedSRGs.Clear();

		for (VulkanShaderResourceGroup* srg : srgs)
		{
			sharedSRGs.Add(srg->GetSRGName(), srg);

			variableNames.AddRange(srg->variableNames);
			setBindings.AddRange(srg->setBindings);
			
			variableNameToBinding.AddRange(srg->variableNameToBinding);
			bindingSlotToBinding.AddRange(srg->bindingSlotToBinding);
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
			return -2;
		}

		BindAllVariables();
	}

	bool VulkanDescriptorSet::ContainsSRG(VulkanShaderResourceGroup* srg)
	{
		if (srg == nullptr)
			return false;
		
		for (const VkDescriptorSetLayoutBinding& setBinding : srg->setBindings)
		{
			if (!setBindings.Exists([&](const VkDescriptorSetLayoutBinding& x) { 
				return x.binding == setBinding.binding && x.descriptorCount == setBinding.descriptorCount &&
					x.descriptorType == setBinding.descriptorType && x.pImmutableSamplers == setBinding.pImmutableSamplers &&
					x.stageFlags == setBinding.stageFlags;
			}))
			{
				return false;
			}
		}

		for (const auto& [bindingSlot, bufferInfo] : srg->bufferVariablesBoundByBindingSlot)
		{
			if (!bufferVariablesBoundByBindingSlot.KeyExists(bindingSlot))
				return false;
			const VkDescriptorBufferInfo& thisBufferInfo = bufferVariablesBoundByBindingSlot[bindingSlot];
			if (thisBufferInfo.buffer != bufferInfo.buffer || thisBufferInfo.offset != bufferInfo.offset || thisBufferInfo.range != bufferInfo.range)
				return false;
		}

		for (const auto& [bindingSlot, imageInfo] : srg->imageVariablesBoundByBindingSlot)
		{
			if (!imageVariablesBoundByBindingSlot.KeyExists(bindingSlot))
				return false;
			const VkDescriptorImageInfo& thisImageInfo = imageVariablesBoundByBindingSlot[bindingSlot];
			if (thisImageInfo.imageLayout != imageInfo.imageLayout || thisImageInfo.imageView != imageInfo.imageView ||
				thisImageInfo.sampler != imageInfo.sampler)
				return false;
		}

		return true;
	}

	int VulkanDescriptorSet::SetSRG(VulkanShaderResourceGroup* srg)
	{
		if (srg == nullptr)
			return -1;
		if (ContainsSRG(srg))
			return 0;
		
		RemoveSRG(srg, false);
		return AddSRG(srg);
	}

	int VulkanDescriptorSet::RemoveSRG(VulkanShaderResourceGroup* srg, bool recreate)
	{
		if (srg == nullptr)
			return -1;

		for (const auto& name : srg->variableNames)
		{
			variableNames.Remove(name);
		}
		for (const auto& setBinding : srg->setBindings)
		{
			setBindings.RemoveFirst([&](const VkDescriptorSetLayoutBinding& b) { return b.binding == setBinding.binding; });
			bufferVariablesBoundByBindingSlot.Remove(setBinding.binding);
			imageVariablesBoundByBindingSlot.Remove(setBinding.binding);
		}

		for (const auto& [name, setBinding] : srg->variableNameToBinding)
		{
			variableNameToBinding.Remove(name);
		}
		for (const auto& [bindingSlot, setBinding] : srg->bindingSlotToBinding)
		{
			bindingSlotToBinding.Remove(bindingSlot);
		}

		for (const auto& [bindingSlot, bufferInfo] : srg->bufferVariablesBoundByBindingSlot)
		{
			bufferVariablesBoundByBindingSlot.Remove(bindingSlot);
		}
		for (const auto& [bindingSlot, imageInfo] : srg->imageVariablesBoundByBindingSlot)
		{
			imageVariablesBoundByBindingSlot.Remove(bindingSlot);
		}

		if (recreate)
			return Recreate();
		return 0;
	}

	int VulkanDescriptorSet::RemoveSRGs(const Array<VulkanShaderResourceGroup*>& srgs, bool recreate)
	{
		if (srgs.IsEmpty())
			return 0;

		for (auto srg : srgs)
		{
			RemoveSRG(srg, false);
		}

		return Recreate();
	}

	int VulkanDescriptorSet::AddSRG(VulkanShaderResourceGroup* srg)
	{
		if (srg == nullptr)
			return -1;

		variableNames.AddRange(srg->variableNames);
		setBindings.AddRange(srg->setBindings);

		variableNameToBinding.AddRange(srg->variableNameToBinding);
		bindingSlotToBinding.AddRange(srg->bindingSlotToBinding);

		bufferVariablesBoundByBindingSlot.AddRange(srg->bufferVariablesBoundByBindingSlot);
		imageVariablesBoundByBindingSlot.AddRange(srg->imageVariablesBoundByBindingSlot);

		return Recreate();
	}

	int VulkanDescriptorSet::Recreate()
	{
		Destroy();

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
			return -2;
		}

		BindAllVariables();

		return 0;
	}

	bool VulkanDescriptorSet::BindVariable(int bindingSlot, RHI::Buffer* buffer, SIZE_T offset, SIZE_T size)
	{
		const auto& bindingInfo = bindingSlotToBinding[bindingSlot];

		// Binding slot already bound. Recreate descriptor set to bind it.
		if (bufferVariablesBoundByBindingSlot.KeyExists(bindingSlot) &&
			bufferVariablesBoundByBindingSlot[bindingSlot].buffer != nullptr)
		{
			bool success = RecreateDescriptorSetWithoutBindingSlot(bindingSlot);
			if (!success)
				return false;
		}

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = (VkBuffer)buffer->GetHandle();
		bufferInfo.offset = offset;
		bufferInfo.range = size > 0 ? size : buffer->GetBufferSize();

		bufferVariablesBoundByBindingSlot[bindingSlot] = bufferInfo;

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptorSet;
		write.dstBinding = bindingSlot;
		write.dstArrayElement = 0;

		write.descriptorCount = bindingInfo.descriptorCount;
		write.descriptorType = bindingInfo.descriptorType;
		write.pBufferInfo = &bufferInfo;
		
		vkUpdateDescriptorSets(device->GetHandle(), 1, &write, 0, nullptr);

		return true;
	}

	bool VulkanDescriptorSet::RecreateDescriptorSetWithoutBindingSlot(int excludeBindingSlot)
	{
		auto pool = device->GetDescriptorPool();
		pool->Free({ descriptorSet });
		descriptorSet = nullptr;

		auto sets = pool->Allocate(1, { setLayout }, descriptorPool);
		if (sets.IsEmpty())
		{
			return false;
		}

		descriptorSet = sets[0];

		Array<VkWriteDescriptorSet> writes{};
		writes.Resize(bufferVariablesBoundByBindingSlot.GetSize() + imageVariablesBoundByBindingSlot.GetSize());
		int count = 0;

		for (const auto& [bindingSlot, bufferInfo] : bufferVariablesBoundByBindingSlot)
		{
			auto buffer = bufferInfo.buffer;
			if (buffer == nullptr || bindingSlot == excludeBindingSlot || bindingSlot < 0)
				continue;
			if (!bindingSlotToBinding.KeyExists(bindingSlot))
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
			if (bindingSlot == excludeBindingSlot || bindingSlot < 0)
				continue;
			if (!bindingSlotToBinding.KeyExists(bindingSlot))
				continue;

			VkDescriptorSetLayoutBinding bindingInfo = bindingSlotToBinding[bindingSlot];

			writes[count] = {};
			writes[count].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writes[count].dstSet = descriptorSet;
			writes[count].dstBinding = bindingSlot;
			writes[count].dstArrayElement = 0;

			writes[count].descriptorCount = bindingInfo.descriptorCount;
			writes[count].descriptorType = bindingInfo.descriptorType;
			writes[count].pBufferInfo = nullptr;
			writes[count].pImageInfo = &imageInfo;

			count++;
		}

		vkUpdateDescriptorSets(device->GetHandle(), count, writes.GetData(), 0, nullptr);

		return true;
	}

	void VulkanDescriptorSet::BindAllVariables()
	{
		if (bufferVariablesBoundByBindingSlot.IsEmpty() && 
			imageVariablesBoundByBindingSlot.IsEmpty())
			return;

		Array<VkWriteDescriptorSet> writes{};
		writes.Resize(bufferVariablesBoundByBindingSlot.GetSize() + 
			imageVariablesBoundByBindingSlot.GetSize());
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

