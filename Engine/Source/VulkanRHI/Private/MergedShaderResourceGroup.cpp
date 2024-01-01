#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    MergedShaderResourceGroup::MergedShaderResourceGroup(VulkanDevice* device, const Array<RHI::ShaderResourceGroup*>& shaderResourceGroups)
		: ShaderResourceGroup(device)
    {
		srgLayout = {};
		int curSrgType = (int)RHI::SRGType::PerScene;
		setNumber = -1;

		for (RHI::ShaderResourceGroup* srg : shaderResourceGroups)
		{
			Vulkan::ShaderResourceGroup* vulkanSRG = (Vulkan::ShaderResourceGroup*)srg;

			if (setNumber == -1)
			{
				setNumber = vulkanSRG->setNumber;
			}
			else if (setNumber != vulkanSRG->setNumber)
			{
				failed = true;
				CE_LOG(Error, All, "");
				return;
			}

			// All input SRGs must be compiled before being merged into a single set.
			if (!srg->IsCompiled())
			{
				failed = true;
				CE_LOG(Error, All, "Failed to create MergedShaderResourceGroup. Make sure to call Compile() on each SRG before creating a Merged SRG.");
				return;
			}

			if ((int)srg->GetSRGType() > curSrgType)
				curSrgType = (int)srg->GetSRGType();

			srgLayout.Merge(srg->GetLayout());

			// Merge bindings
			bufferInfosBoundBySlot.AddRange(vulkanSRG->bufferInfosBoundBySlot);
			imageInfosBoundBySlot.AddRange(vulkanSRG->imageInfosBoundBySlot);
		}

		this->srgType = (RHI::SRGType)curSrgType;

		Compile();
    }

    MergedShaderResourceGroup::~MergedShaderResourceGroup()
    {
    }

} // namespace CE::Vulkan
