#pragma once

namespace CE
{
	class VulkanShaderResourceGroup;

	class VULKANRHI_API VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet(VulkanDevice* device, const Array<VulkanShaderResourceGroup*>& srgs);

	private:
        
        int frequencyId = 0;
        Name srgName{};
        RHI::SRGType srgType{};

		VulkanDevice* device = nullptr;
		VkDescriptorPool descriptorPool = nullptr;
		VkDescriptorSet descriptorSet = nullptr;

        Array<Name> variableNames{};
        Array<VkDescriptorSetLayoutBinding> bindings{};

        HashMap<Name, VkDescriptorSetLayoutBinding> variableNameToBinding{};
        HashMap<int, VkDescriptorSetLayoutBinding> bindingSlotToBinding{};

        HashMap<int, VkDescriptorBufferInfo> bufferVariablesBoundByBindingSlot{};
	};

} // namespace CE
