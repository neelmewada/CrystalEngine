#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
	CommandList::CommandList(VulkanDevice* device, VkCommandBuffer commandBuffer, VkCommandBufferLevel level, u32 queueFamilyIndex, VkCommandPool pool)
		: device(device)
		, commandBuffer(commandBuffer)
		, level(level)
		, queueFamilyIndex(queueFamilyIndex)
		, pool(pool)
	{
		srgManager = device->GetShaderResourceManager();
	}

	CommandList::~CommandList()
	{
		vkFreeCommandBuffers(device->GetHandle(), pool, 1, &commandBuffer);
	}

	void CommandList::SetShaderResourceGroups(const ArrayView<RHI::ShaderResourceGroup*>& srgs)
	{
		for (auto rhiSrg : srgs)
		{
			auto srg = (Vulkan::ShaderResourceGroup*)rhiSrg;
			int setNumber = srg->GetSetNumber();

			if (boundSRGs[setNumber] != nullptr)
			{
				boundSRGs[setNumber] = srgManager->FindOrCreateMergedSRG({ boundSRGs[setNumber], srg });
			}
			else
			{
				boundSRGs[setNumber] = srg;
			}
		}
	}

} // namespace CE::Vulkan
