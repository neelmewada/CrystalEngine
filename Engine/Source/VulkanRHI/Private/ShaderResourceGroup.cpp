#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

	ShaderResourceManager::ShaderResourceManager(VulkanDevice* device)
	{
		const auto& limits = device->GetDeviceLimits();
		maxBoundDescriptorSets =  limits.maxBoundDescriptorSets;
        
        if (maxBoundDescriptorSets == 4)
        {
            srgSlots.Add({ RHI::SRGType::PerScene, 0 });
            srgSlots.Add({ RHI::SRGType::PerView, 0 });
            srgSlots.Add({ RHI::SRGType::PerPass, 0 });
            srgSlots.Add({ RHI::SRGType::PerSubPass, 0 });
            srgSlots.Add({ RHI::SRGType::PerMaterial, 1 });
            srgSlots.Add({ RHI::SRGType::PerObject, 2 });
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

	ShaderResourceManager::~ShaderResourceManager()
	{
	}

	bool ShaderResourceManager::IsSharedDescriptorSet(RHI::SRGType srgType)
	{
		if (!builtinSrgNameToDescriptorSet.KeyExists(srgType))
			return false;

		int set = builtinSrgNameToDescriptorSet[srgType].set;
		return IsSharedDescriptorSet(set);
	}

	int ShaderResourceManager::GetDescriptorSetNumber(RHI::SRGType srgType)
	{
		if (!builtinSrgNameToDescriptorSet.KeyExists(srgType))
			return false;

		return builtinSrgNameToDescriptorSet[srgType].set;
	}

	bool ShaderResourceManager::ShouldCreateDescriptorSetForSRG(RHI::SRGType srgType)
	{
		if (!builtinSrgNameToDescriptorSet.KeyExists(srgType))
			return false;

		int set = builtinSrgNameToDescriptorSet[srgType].set;
		const auto& array = descriptorSetToSrgs[set];
		if (array.IsEmpty())
			return false;

		return array.GetLast().srgType == srgType;
	}

	VkDescriptorType ShaderResourceManager::GetDescriptorType(ShaderResourceType shaderResourceType, bool usesDynamicOffset)
	{
		switch (shaderResourceType)
		{
		case ShaderResourceType::ConstantBuffer:
			return usesDynamicOffset ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case ShaderResourceType::StructuredBuffer:
		case ShaderResourceType::RWStructuredBuffer:
			return usesDynamicOffset ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case ShaderResourceType::Texture1D:
		case ShaderResourceType::Texture2D:
		case ShaderResourceType::Texture3D:
		case ShaderResourceType::TextureCube:
			return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		case ShaderResourceType::RWTexture2D:
		case ShaderResourceType::RWTexture3D:
			return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		case ShaderResourceType::InputAttachment:
			return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		case ShaderResourceType::SamplerState:
			return VK_DESCRIPTOR_TYPE_SAMPLER;
		}

		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}

    ShaderResourceGroup::ShaderResourceGroup(VulkanDevice* device, const RHI::ShaderResourceGroupLayout& srgLayout)
		: device(device)
    {
		this->srgType = srgLayout.srgType;
		this->srgLayout = srgLayout;
		this->srgManager = device->GetShaderResourceManager();
		pool = device->GetDescriptorPool();
    }

    ShaderResourceGroup::~ShaderResourceGroup()
	{
		if (descriptorSet)
		{
			pool->Free({ descriptorSet });
			descriptorSet = null;
		}

		if (setLayout)
		{
			vkDestroyDescriptorSetLayout(device->GetHandle(), setLayout, null);
			setLayout = null;
		}
	}

	ShaderResourceGroup::ShaderResourceGroup(VulkanDevice* device)
		: device(device)
	{
		this->srgManager = device->GetShaderResourceManager();
		pool = device->GetDescriptorPool();
	}

	bool ShaderResourceGroup::Bind(Name name, RHI::Buffer* buffer, SIZE_T offset, SIZE_T size)
	{
		if (isCompiled)
			return false;
		if (!bindingSlotsByVariableName.KeyExists(name))
			return false;

		VkDescriptorBufferInfo bufferWrite{};
		bufferWrite.buffer = (VkBuffer)buffer->GetHandle();
		bufferWrite.offset = offset;
		bufferWrite.range = size > 0 ? size : buffer->GetBufferSize();
		
		int bindingSlot = bindingSlotsByVariableName[name];
		buffersBoundBySlot[bindingSlot] = bufferWrite;
		
		return true;
	}

	void ShaderResourceGroup::Compile()
	{
		if (isCompiled)
			return;

		isCompiled = true;

		setLayoutBindings.Clear();
		bindingSlotsByVariableName.Clear();
		variableBindingsByName.Clear();
		variableBindingsBySlot.Clear();

		for (const RHI::SRGVariableDesc& variable : srgLayout.variables)
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
			CE_LOG(Error, All, "Failed to create vulkan descriptor set layout");
			return;
		}

		auto allocatedSets = pool->Allocate(1, { setLayout }, allocPool);
		if (allocatedSets.IsEmpty())
		{
			CE_LOG(Error, All, "Failed to allocate descriptor set");
			return;
		}

		descriptorSet = allocatedSets[0];

		CompileBindings();
	}

	void ShaderResourceGroup::Destroy()
	{
		if (descriptorSet)
		{
			pool->Free({ descriptorSet });
			descriptorSet = null;
		}

		if (setLayout)
		{
			vkDestroyDescriptorSetLayout(device->GetHandle(), setLayout, null);
			setLayout = null;
		}
	}

	void ShaderResourceGroup::CompileBindings()
	{
		Array<VkWriteDescriptorSet> writes{};
		writes.Resize(buffersBoundBySlot.GetSize() + imagesBoundBySlot.GetSize());
		int idx = 0;

		for (const auto& [slot, bufferWrite] : buffersBoundBySlot)
		{
			VkDescriptorSetLayoutBinding& variable = variableBindingsBySlot[slot];

			VkWriteDescriptorSet& write = writes[idx++];
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.descriptorCount = variable.descriptorCount;
			write.descriptorType = variable.descriptorType;
			write.dstArrayElement = 0;
			write.dstBinding = variable.binding;
			write.dstSet = descriptorSet;
			write.pBufferInfo = &bufferWrite;
		}

		for (const auto& [slot, imageWrite] : imagesBoundBySlot)
		{
			VkDescriptorSetLayoutBinding& variable = variableBindingsBySlot[slot];

			VkWriteDescriptorSet& write = writes[idx++];
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.descriptorCount = variable.descriptorCount;
			write.descriptorType = variable.descriptorType;
			write.dstArrayElement = 0;
			write.dstBinding = variable.binding;
			write.dstSet = descriptorSet;
			write.pImageInfo = &imageWrite;
		}

		vkUpdateDescriptorSets(device->GetHandle(), writes.GetSize(), writes.GetData(), 0, null);
	}

} // namespace CE

