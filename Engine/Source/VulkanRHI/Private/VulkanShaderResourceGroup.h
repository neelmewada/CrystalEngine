#pragma once

namespace CE
{

	class VulkanShaderResourceManager
	{
	public:

		VulkanShaderResourceManager(VulkanDevice* device);
		~VulkanShaderResourceManager();

		inline u32 GetMaxBoundSets() const { return maxBoundDescriptorSets; }

	private:

		HashMap<int, Array<RHI::SRGType>> srgTypesByFrequency{};
		
		u32 maxBoundDescriptorSets = 0;

	};

	class VulkanShaderResourceGroup : public RHI::ShaderResourceGroup
	{
	public:

		VulkanShaderResourceGroup(VulkanDevice* device, const RHI::ShaderResourceGroupDesc& desc);

		~VulkanShaderResourceGroup();

		virtual bool Bind(Name variableName, RHI::Buffer* buffer) override;

		virtual int GetFrequencyId() override;

		virtual Name GetSRGName() override;

		virtual RHI::SRGType GetSRGType() override;

		inline VkDescriptorSet GetDescriptorSet() const
		{
			return descriptorSet;
		}

		/// @brief Recreates the descriptor sets and rebinds all bindings that were already bound in the descriptor set except for the given binding slot.
		/// @param excludeBindingSlot: The binding slot to not re-bind.
		/// @return true if succeeds.
		bool RecreateDescriptorSetWithoutBindingSlot(int excludeBindingSlot);

	private:

		int frequencyId = 0;
		Name srgName{};
		RHI::SRGType srgType{};

		VulkanDevice* device = nullptr;
		VkDescriptorSetLayout setLayout = nullptr;

		VkDescriptorSet descriptorSet = nullptr;
		VkDescriptorPool descriptorPool = nullptr;

		RHI::ShaderResourceGroupDesc desc{};

		Array<Name> variableNames{};
		Array<VkDescriptorSetLayoutBinding> bindings{};

		HashMap<Name, VkDescriptorSetLayoutBinding> variableNameToBinding{};
		HashMap<int, VkDescriptorSetLayoutBinding> bindingSlotToBinding{};

		HashMap<int, VkDescriptorBufferInfo> bufferVariablesBoundByBindingSlot{};

		friend class VulkanGraphicsPipeline;
		friend class VulkanGraphicsCommandList;
	};

} // namespace CE
