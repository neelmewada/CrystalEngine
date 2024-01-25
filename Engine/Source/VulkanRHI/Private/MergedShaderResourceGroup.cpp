#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    MergedShaderResourceGroup::MergedShaderResourceGroup(VulkanDevice* device, const ArrayView<Vulkan::ShaderResourceGroup*>& shaderResourceGroups)
		: ShaderResourceGroup(device)
    {
		srgLayout = {};
		int curSrgType = (int)RHI::SRGType::PerScene;
		setNumber = -1;

		if (shaderResourceGroups.IsEmpty())
			return;

		int i = 0;
		combinedSRGs.Resize(shaderResourceGroups.GetSize());

		for (Vulkan::ShaderResourceGroup* srg : shaderResourceGroups)
		{
			combinedSRGs[i++] = srg;
			srgManager->mergedSRGsBySourceSRG[srg].Add(this);

			if (setNumber == -1)
			{
				setNumber = srg->setNumber;
			}
			else if (setNumber != srg->setNumber)
			{
				failed = true;
				CE_LOG(Error, All, "");
				return;
			}

			// All input SRGs must be compiled before being merged into a single set.
			if (!srg->IsCompiled())
			{
				srg->Compile();
			}

			if ((int)srg->GetSRGType() > curSrgType)
				curSrgType = (int)srg->GetSRGType();

			srgLayout.Merge(srg->GetLayout());

			// Merge bindings
			bufferInfosBoundBySlot.AddRange(srg->bufferInfosBoundBySlot);
			imageInfosBoundBySlot.AddRange(srg->imageInfosBoundBySlot);
		}

		combinedSRGs.Sort([](Vulkan::ShaderResourceGroup* a, Vulkan::ShaderResourceGroup* b)
			{
				return (int)a->GetSRGType() <= (int)b->GetSRGType();
			});

		mergedHash = combinedSRGs[0]->GetHash();
		for (int i = 1; i < combinedSRGs.GetSize(); i++)
		{
			mergedHash = GetCombinedHash(mergedHash, combinedSRGs[i]->GetHash());
		}

		this->srgType = (RHI::SRGType)curSrgType;

		Compile();
    }

    MergedShaderResourceGroup::~MergedShaderResourceGroup()
    {
		// Remove this SRG from SRGManager
		srgManager->RemoveMergedSRG(this);
    }

} // namespace CE::Vulkan
