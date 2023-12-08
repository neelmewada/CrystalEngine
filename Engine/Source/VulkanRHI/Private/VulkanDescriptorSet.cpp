#include "VulkanRHIPrivate.h"

#include "VulkanDescriptorSet.h"
#include "VulkanShaderResourceGroup.h"

namespace CE
{

	VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice* device, const Array<VulkanShaderResourceGroup*>& srgs)
		: device(device)
	{
        frequencyId = desc.frequencyId;
        srgName = desc.srgName;
        srgType = desc.srgType;
        
        VkDescriptorSetLayoutCreateInfo setLayoutCI{};
        setLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

        for (VulkanShaderResourceGroup* srg : srgs)
        {
            srg->
        }
	}

} // namespace CE

