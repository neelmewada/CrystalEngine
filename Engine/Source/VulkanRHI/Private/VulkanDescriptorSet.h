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
		int Create(Array<VulkanShaderResourceGroup*> sharedResourceGroups);

		/// @brief Returns true if the given srg is a subset of this descriptor set.
		/// @param srg 
		/// @return 
		bool ContainsSRG(VulkanShaderResourceGroup* srg);

		/// @brief Sets the given SRG into this shared descriptor set. Always use SetSRG() function instead of AddSRG().
		/// @param srg 
		/// @return 
		int SetSRG(VulkanShaderResourceGroup* srg);

		bool BindVariable(int bindingSlot, RHI::Buffer* buffer, SIZE_T offset, SIZE_T size);

		inline int RemoveSRG(VulkanShaderResourceGroup* srg)
		{
			return RemoveSRG(srg, true);
		}

		inline int RemoveSRGs(const Array<VulkanShaderResourceGroup*>& srgs)
		{
			return RemoveSRGs(srgs, true);
		}

	private:

		int RemoveSRG(VulkanShaderResourceGroup* srg, bool recreate);

		int RemoveSRGs(const Array<VulkanShaderResourceGroup*>& srgs, bool recreate);

		int AddSRG(VulkanShaderResourceGroup* srg);

		int Recreate();

		bool RecreateDescriptorSetWithoutBindingSlot(int excludeBindingSlot);

		void BindAllVariables();

		VulkanDevice* device = nullptr;
		VkDescriptorPool descriptorPool = nullptr;
		VkDescriptorSet descriptorSet = nullptr;
		VkDescriptorSetLayout setLayout = nullptr;

		HashMap<Name, VulkanShaderResourceGroup*> boundSRGs{};

        Array<Name> variableNames{};
        Array<VkDescriptorSetLayoutBinding> setBindings{};

        HashMap<Name, VkDescriptorSetLayoutBinding> variableNameToBinding{};
        HashMap<int, VkDescriptorSetLayoutBinding> bindingSlotToBinding{};

		// Bound buffers/images
		HashMap<int, VkDescriptorBufferInfo> bufferVariablesBoundByBindingSlot{};
		HashMap<int, VkDescriptorImageInfo> imageVariablesBoundByBindingSlot{};

		friend class VulkanShaderResourceGroup;
	};

} // namespace CE
