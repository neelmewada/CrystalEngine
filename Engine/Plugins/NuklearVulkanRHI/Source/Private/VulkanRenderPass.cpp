
#include "VulkanRenderPass.h"

namespace CE
{


	VulkanRenderPass::VulkanRenderPass(VulkanDevice* device, const VulkanRenderTargetLayout& rtLayout)
		: device(device), rtLayout(rtLayout)
	{


		CE_LOG(Info, All, "Vulkan RenderPass created");
	}

	VulkanRenderPass::~VulkanRenderPass()
	{


		CE_LOG(Info, All, "Vulkan RenderPass destroyed");
	}

} // namespace CE

