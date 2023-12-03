#pragma once

namespace CE
{
    
	class VULKANRHI_API VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet(VulkanDevice* device, const RHI::ShaderResourceGroupDesc& desc);

	private:

		VulkanDevice* device = nullptr;
		VkDescriptorPool descriptorPool = nullptr;
		VkDescriptorSet descriptorSet = nullptr;

	};

} // namespace CE
