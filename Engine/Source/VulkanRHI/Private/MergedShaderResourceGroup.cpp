#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    MergedShaderResourceGroup::MergedShaderResourceGroup(VulkanDevice* device, const Array<RHI::ShaderResourceGroup*>& shaderResourceGroups)
		: ShaderResourceGroup(device)
    {
		srgLayout = {};
		int curSrgType = (int)RHI::SRGType::PerScene;

		for (RHI::ShaderResourceGroup* srg : shaderResourceGroups)
		{
			if ((int)srg->GetSRGType() > curSrgType)
				curSrgType = (int)srg->GetSRGType();


		}

		this->srgType = (RHI::SRGType)curSrgType;
    }

    MergedShaderResourceGroup::~MergedShaderResourceGroup()
    {
    }

} // namespace CE::Vulkan
