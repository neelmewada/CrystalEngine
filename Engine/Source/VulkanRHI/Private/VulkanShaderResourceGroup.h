#pragma once

namespace CE
{
	class VulkanDescriptorSet;
	class VulkanShaderResourceGroup;

	class VulkanShaderResourceManager
	{
	public:

		VulkanShaderResourceManager(VulkanDevice* device);
		~VulkanShaderResourceManager();

		inline u32 GetMaxBoundSets() const { return maxBoundDescriptorSets; }
        
        inline bool IsSharedDescriptorSet(int set) { return descriptorSetToSrgs[set].GetSize() > 1; }
		bool IsSharedDescriptorSet(RHI::SRGType srgType);

		int GetDescriptorSetNumber(RHI::SRGType srgType);

		bool ShouldCreateDescriptorSetForSRG(RHI::SRGType srgType);

	private:
        
        struct SRGSlot
        {
            RHI::SRGType srgType{};
            int set;
        };
        
        Array<SRGSlot> srgSlots{};
        
        HashMap<RHI::SRGType, SRGSlot> builtinSrgNameToDescriptorSet{};
		HashMap<int, Array<SRGSlot>> descriptorSetToSrgs{};
		
		u32 maxBoundDescriptorSets = 0;

	};

	class VulkanShaderResourceGroup : public RHI::ShaderResourceGroup
	{
	public:

		VulkanShaderResourceGroup(VulkanDevice* device, const RHI::ShaderResourceGroupDesc& desc);

		~VulkanShaderResourceGroup();

		virtual bool Bind(Name variableName, RHI::Buffer* buffer, SIZE_T offset = 0, SIZE_T size = 0) override;

		virtual int GetFrequencyId() override;

		virtual Name GetSRGName() override;

		virtual RHI::SRGType GetSRGType() override;

		virtual const RHI::ShaderResourceGroupDesc& GetDesc() override;

		inline VkDescriptorSet GetDescriptorSet() const
		{
			if (sharedDescriptorSet == nullptr)
				return nullptr;
			return sharedDescriptorSet->descriptorSet;
		}

		inline bool ManagesDescriptorSet() const
		{
			return sharedDescriptorSet != nullptr;
		}

	private:

		int setNumber = 0;
		Name srgName{};
		RHI::SRGType srgType{};
		bool isCommitted = false;

		VulkanDevice* device = nullptr;

		VulkanShaderResourceManager* srgManager = nullptr;
		VulkanDescriptorSet* sharedDescriptorSet = nullptr;

		RHI::ShaderResourceGroupDesc desc{};

		Array<Name> variableNames{};
		Array<VkDescriptorSetLayoutBinding> setBindings{};

		HashMap<Name, VkDescriptorSetLayoutBinding> variableNameToBinding{};
		HashMap<int, VkDescriptorSetLayoutBinding> bindingSlotToBinding{};

		// Bound buffers/images
		HashMap<int, VkDescriptorBufferInfo> bufferVariablesBoundByBindingSlot{};
		HashMap<int, VkDescriptorImageInfo> imageVariablesBoundByBindingSlot{};

		friend class VulkanGraphicsPipeline;
		friend class VulkanGraphicsCommandList;
        friend class VulkanDescriptorSet;
	};

} // namespace CE
