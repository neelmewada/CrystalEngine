#include "VulkanRHIPrivate.h"

#include "VulkanDevice.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"
#include "VulkanShaderResourceGroup.h"

namespace CE
{

	VulkanShaderResourceManager::VulkanShaderResourceManager(VulkanDevice* device)
	{
		const auto& limits = device->GetDeviceLimits();
		maxBoundDescriptorSets =  limits.maxBoundDescriptorSets;
        
        if (maxBoundDescriptorSets == 4)
        {
            srgSlots.Add({ RHI::SRGType::PerScene, 0 });
            srgSlots.Add({ RHI::SRGType::PerView, 1 });
            srgSlots.Add({ RHI::SRGType::PerPass, 1 });
            srgSlots.Add({ RHI::SRGType::PerSubPass, 1 });
            srgSlots.Add({ RHI::SRGType::PerMaterial, 2 });
            srgSlots.Add({ RHI::SRGType::PerObject, 3 });
            srgSlots.Add({ RHI::SRGType::PerDraw, 3 });
        }
        else if (maxBoundDescriptorSets == 5)
        {
            srgSlots.Add({ RHI::SRGType::PerScene, 0 });
            srgSlots.Add({ RHI::SRGType::PerView, 1 });
            srgSlots.Add({ RHI::SRGType::PerPass, 1 });
            srgSlots.Add({ RHI::SRGType::PerSubPass, 1 });
            srgSlots.Add({ RHI::SRGType::PerMaterial, 2 });
            srgSlots.Add({ RHI::SRGType::PerObject, 3 });
            srgSlots.Add({ RHI::SRGType::PerDraw, 4 });
        }
        else if (maxBoundDescriptorSets == 6)
        {
            srgSlots.Add({ RHI::SRGType::PerScene, 0 });
            srgSlots.Add({ RHI::SRGType::PerView, 1 });
            srgSlots.Add({ RHI::SRGType::PerPass, 2 });
            srgSlots.Add({ RHI::SRGType::PerSubPass, 2 });
            srgSlots.Add({ RHI::SRGType::PerMaterial, 3 });
            srgSlots.Add({ RHI::SRGType::PerObject, 4 });
            srgSlots.Add({ RHI::SRGType::PerDraw, 5 });
        }
        else if (maxBoundDescriptorSets >= 7)
        {
            srgSlots.Add({ RHI::SRGType::PerScene, 0 });
            srgSlots.Add({ RHI::SRGType::PerView, 1 });
            srgSlots.Add({ RHI::SRGType::PerPass, 2 });
            srgSlots.Add({ RHI::SRGType::PerSubPass, 3 });
            srgSlots.Add({ RHI::SRGType::PerMaterial, 4 });
            srgSlots.Add({ RHI::SRGType::PerObject, 5 });
            srgSlots.Add({ RHI::SRGType::PerDraw, 6 });
        }
        
        for (const SRGSlot& slot : srgSlots)
        {
            builtinSrgNameToDescriptorSet[slot.srgType] = slot;
            descriptorSetToSrgs[slot.set].Add(slot);
        }
	}

	VulkanShaderResourceManager::~VulkanShaderResourceManager()
	{
	}

	VulkanShaderResourceGroup::VulkanShaderResourceGroup(VulkanDevice* device, const RHI::ShaderResourceGroupDesc& desc)
		: device(device), desc(desc)
	{
		frequencyId = desc.frequencyId;
		srgName = desc.srgName;
		srgType = desc.srgType;
		
		VkDescriptorSetLayoutCreateInfo setLayoutCI{};
		setLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

		setBindings.Clear();
		variableNames.Clear();

		for (int i = 0; i < desc.variables.GetSize(); i++)
		{
			const auto& variable = desc.variables[i];

			VkDescriptorSetLayoutBinding binding{};
			binding.binding = variable.binding;
			binding.descriptorCount = variable.arrayCount;
			binding.stageFlags = 0;

			if (EnumHasFlag(variable.stageFlags, RHI::ShaderStage::Vertex))
				binding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
			if (EnumHasFlag(variable.stageFlags, RHI::ShaderStage::Fragment))
				binding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;

			if (binding.stageFlags == 0)
				continue;

			switch (variable.type)
			{
			case RHI::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER:
				binding.descriptorType = variable.isDynamic ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				break;
			case RHI::SHADER_RESOURCE_TYPE_STRUCTURED_BUFFER:
				binding.descriptorType = variable.isDynamic ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				break;
			case RHI::SHADER_RESOURCE_TYPE_SAMPLED_IMAGE:
				binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				break;
			case RHI::SHADER_RESOURCE_TYPE_SAMPLER_STATE:
				binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				break;
			default:
				continue;
			}

			variableNameToBinding[variable.name] = binding;
			bindingSlotToBinding[variable.binding] = binding;
			variableNames.Add(variable.name);
			setBindings.Add(binding);
		}

		setLayoutCI.bindingCount = setBindings.GetSize();
		setLayoutCI.pBindings = setBindings.GetData();

		auto result = vkCreateDescriptorSetLayout(device->GetHandle(), &setLayoutCI, nullptr, &setLayout);
		if (result != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Descriptor Set Layout");
			return;
		}

		auto pool = device->GetDescriptorPool();

		auto sets = pool->Allocate(1, { setLayout }, descriptorPool);
		if (sets.IsEmpty())
		{
			return;
		}

		descriptorSet = sets[0];
	}

	VulkanShaderResourceGroup::~VulkanShaderResourceGroup()
	{
		vkFreeDescriptorSets(device->GetHandle(), descriptorPool, 1, &descriptorSet);
		descriptorPool = nullptr;
		descriptorSet = nullptr;

		vkDestroyDescriptorSetLayout(device->GetHandle(), setLayout, nullptr);
		setLayout = nullptr;
	}

	bool VulkanShaderResourceGroup::Bind(Name variableName, RHI::Buffer* buffer, SIZE_T offset = 0, SIZE_T size = 0)
	{
		int index = variableNames.IndexOf(variableName);
		if (index < 0)
			return false;

		VkDescriptorSetLayoutBinding bindingInfo = setBindings[index];
		int bindingSlot = bindingInfo.binding;

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

	int VulkanShaderResourceGroup::GetFrequencyId()
	{
		return frequencyId;
	}

	Name VulkanShaderResourceGroup::GetSRGName()
	{
		return srgName;
	}

	RHI::SRGType VulkanShaderResourceGroup::GetSRGType()
	{
		return srgType;
	}

	bool VulkanShaderResourceGroup::RecreateDescriptorSetWithoutBindingSlot(int excludeBindingSlot)
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

} // namespace CE

