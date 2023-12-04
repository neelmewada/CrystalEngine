#include "VulkanRHIPrivate.h"

#include "VulkanDevice.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"
#include "VulkanShaderResourceGroup.h"

namespace CE
{

	VulkanShaderResourceGroup::VulkanShaderResourceGroup(VulkanDevice* device, const RHI::ShaderResourceGroupDesc& desc)
		: device(device), desc(desc)
	{
		frequencyId = desc.frequencyId;
		srgName = desc.srgName;
		srgType = desc.srgType;

		VkDescriptorSetLayoutCreateInfo setLayoutCI{};
		setLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

		bindings.Clear();
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
			bindings.Add(binding);
		}

		setLayoutCI.bindingCount = bindings.GetSize();
		setLayoutCI.pBindings = bindings.GetData();

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

	bool VulkanShaderResourceGroup::Bind(Name variableName, RHI::Buffer* buffer)
	{
		int index = variableNames.IndexOf(variableName);
		if (index < 0)
			return false;

		VkDescriptorSetLayoutBinding bindingInfo = bindings[index];
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
		bufferInfo.offset = 0;
		bufferInfo.range = buffer->GetBufferSize();

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
		writes.Resize(bufferVariablesBoundByBindingSlot.GetSize());
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

		vkUpdateDescriptorSets(device->GetHandle(), count, writes.GetData(), 0, nullptr);

		return true;
	}
    
} // namespace CE
