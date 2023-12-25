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

	ShaderResourceGroup::ShaderResourceGroup(VulkanDevice* device, const RHI::ShaderResourceGroupDesc& desc)
		: device(device), desc(desc)
	{
		srgManager = device->GetShaderResourceManager();
		srgName = desc.srgName;
		srgType = desc.srgType;
		setNumber = srgManager->GetDescriptorSetNumber(desc.srgType);

		if ((int)srgType < (int)RHI::SRGType::COUNT)
		{
			auto enumType = GetStaticEnum<RHI::SRGType>();
			auto enumConstant = enumType->FindConstantWithValue((s64)srgType);
			if (enumConstant != nullptr)
			{
				srgName = enumConstant->GetName();
			}
		}

		setBindings.Clear();
		variableNames.Clear();
		
		for (int i = 0; i < desc.variables.GetSize(); i++)
		{
			const auto& variable = desc.variables[i];
			
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = binding; // TODO: Find an alternative way to get bindingSlot number from shader reflection
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
			bindingSlotToBinding[binding.binding] = binding;
			variableNames.Add(variable.name);
			setBindings.Add(binding);
		}

		if (srgManager->ShouldCreateDescriptorSetForSRG(srgType))
		{
			sharedDescriptorSet = new VulkanDescriptorSet(device);
		}
	}

	ShaderResourceGroup::~ShaderResourceGroup()
	{
		delete sharedDescriptorSet;
		sharedDescriptorSet = nullptr;
	}

	bool ShaderResourceGroup::Bind(Name variableName, RHI::Buffer* buffer, SIZE_T offset, SIZE_T size)
	{
		// Cannot change bindings once an SRG is committed
		if (isCommitted)
			return false;

		int index = variableNames.IndexOf(variableName);
		if (index < 0)
			return false;
		
		VkDescriptorSetLayoutBinding bindingInfo = setBindings[index];
		int bindingSlot = bindingInfo.binding;
		
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = (VkBuffer)buffer->GetHandle();
		bufferInfo.offset = offset;
		bufferInfo.range = size > 0 ? size : buffer->GetBufferSize();
		
		bufferVariablesBoundByBindingSlot[bindingSlot] = bufferInfo;
	}

	int ShaderResourceGroup::GetFrequencyId() const
	{
		return setNumber;
	}

	Name ShaderResourceGroup::GetSRGName() const
	{
		return srgName;
	}

	RHI::SRGType ShaderResourceGroup::GetSRGType() const
	{
		return srgType;
	}

	const RHI::ShaderResourceGroupDesc& ShaderResourceGroup::GetDesc() const
	{
		return desc;
	}

    VkDescriptorSet ShaderResourceGroup::GetDescriptorSet() const
    {
		if (sharedDescriptorSet == nullptr)
			return nullptr;
		return sharedDescriptorSet->descriptorSet;
    }

} // namespace CE

