#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    DescriptorSet::DescriptorSet(VulkanDevice* device, const RHI::ShaderResourceGroupLayout& srgLayout)
		: device(device)
    {
		this->srgLayout = srgLayout;
		this->srgManager = device->GetShaderResourceManager();
		pool = device->GetDescriptorPool();
		setNumber = srgManager->GetDescriptorSetNumber(srgLayout.srgType);

		setLayoutBindings.Clear();
		bindingSlotsByVariableName.Clear();
		variableBindingsByName.Clear();
		variableBindingsBySlot.Clear();

		for (const RHI::SRGVariableDescriptor& variable : srgLayout.variables)
		{
			setLayoutBindings.Add({});
			VkDescriptorSetLayoutBinding& entry = setLayoutBindings.GetLast();
			entry.binding = variable.bindingSlot;
			entry.descriptorCount = variable.arrayCount;
			entry.descriptorType = srgManager->GetDescriptorType(variable.type, variable.usesDynamicOffset);
			entry.stageFlags = 0;

			if (EnumHasFlag(variable.shaderStages, RHI::ShaderStage::Vertex))
			{
				entry.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
			}
			if (EnumHasFlag(variable.shaderStages, RHI::ShaderStage::Fragment))
			{
				entry.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			if (EnumHasFlag(variable.shaderStages, RHI::ShaderStage::Tessellation))
			{
				entry.stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			}

			bindingSlotsByVariableName[variable.name] = variable.bindingSlot;
			variableBindingsByName[variable.name] = entry;
			variableBindingsBySlot[variable.bindingSlot] = entry;
		}

		VkDescriptorSetLayoutCreateInfo layoutCI{};
		layoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutCI.bindingCount = setLayoutBindings.GetSize();
		layoutCI.pBindings = setLayoutBindings.GetData();

		auto result = vkCreateDescriptorSetLayout(device->GetHandle(), &layoutCI, nullptr, &setLayout);
		if (result != VK_SUCCESS)
		{
			failed = true;
			CE_LOG(Error, All, "Failed to create vulkan descriptor set layout");
			return;
		}

		auto allocatedSets = pool->Allocate(1, { setLayout }, descriptorPool);
		if (allocatedSets.IsEmpty())
		{
			failed = true;
			CE_LOG(Error, All, "Failed to allocate descriptor set");
			return;
		}

		descriptorSet = allocatedSets[0];
    }

    DescriptorSet::~DescriptorSet()
    {
		vkFreeDescriptorSets(device->GetHandle(), descriptorPool, 1, &descriptorSet);
		descriptorSet = nullptr;

		if (setLayout)
		{
			vkDestroyDescriptorSetLayout(device->GetHandle(), setLayout, nullptr);
			setLayout = nullptr;
		}
    }

} // namespace CE::Vulkan
