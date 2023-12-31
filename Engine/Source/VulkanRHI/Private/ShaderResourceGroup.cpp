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

    ShaderResourceGroup::ShaderResourceGroup(VulkanDevice* device, const RHI::ShaderResourceGroupLayout& srgLayout)
    {
		this->srgLayout = srgLayout;

		List<VkDescriptorSetLayoutBinding> layoutBindings{};

		for (const RHI::SRGVariableDesc& variable : srgLayout.variables)
		{
			layoutBindings.Add({});
			VkDescriptorSetLayoutBinding& entry = layoutBindings.GetLast();
			entry.binding = variable.bindingSlot;
			entry.descriptorCount = variable.arrayCount;
			

		}

		VkDescriptorSetLayoutCreateInfo layoutCI{};
		layoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    }

    ShaderResourceGroup::~ShaderResourceGroup()
	{
		
	}

	bool ShaderResourceGroup::Bind(Name name, RHI::Buffer* buffer, SIZE_T offset, SIZE_T size)
	{
		
		return false;
	}

} // namespace CE

