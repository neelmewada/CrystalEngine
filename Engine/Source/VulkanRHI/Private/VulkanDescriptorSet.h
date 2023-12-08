#pragma once

namespace CE
{
	class VulkanShaderResourceGroup;

	class VULKANRHI_API VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet(VulkanDevice* device);

		~VulkanDescriptorSet();

		void Destroy();

		// Create a descriptor set by combining multiple SRGs
		int Create(const Array<VulkanShaderResourceGroup*>& sharedResourceGroups);

	private:

		void BindAllVariables();

		VulkanDevice* device = nullptr;
		VkDescriptorPool descriptorPool = nullptr;
		VkDescriptorSet descriptorSet = nullptr;
		VkDescriptorSetLayout setLayout = nullptr;

        Array<Name> variableNames{};
        Array<VkDescriptorSetLayoutBinding> setBindings{};

        HashMap<Name, VkDescriptorSetLayoutBinding> variableNameToBinding{};
        HashMap<int, VkDescriptorSetLayoutBinding> bindingSlotToBinding{};

		// Bound buffers/images
		HashMap<int, VkDescriptorBufferInfo> bufferVariablesBoundByBindingSlot{};
		HashMap<int, VkDescriptorImageInfo> imageVariablesBoundByBindingSlot{};

	};

} // namespace CE
