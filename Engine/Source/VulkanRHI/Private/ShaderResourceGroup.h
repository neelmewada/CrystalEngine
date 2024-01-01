#pragma once

namespace CE::Vulkan
{
	class VulkanDescriptorSet;
	class ShaderResourceGroup;

	class ShaderResourceManager
	{
	public:

		ShaderResourceManager(VulkanDevice* device);
		~ShaderResourceManager();

		inline u32 GetMaxBoundSets() const { return maxBoundDescriptorSets; }
        
        inline bool IsSharedDescriptorSet(int set) { return setNumberToSrgs[set].GetSize() > 1; }
		bool IsSharedDescriptorSet(RHI::SRGType srgType);

		int GetDescriptorSetNumber(RHI::SRGType srgType);

		bool ShouldCreateDescriptorSetForSRG(RHI::SRGType srgType);

		VkDescriptorType GetDescriptorType(ShaderResourceType shaderResourceType, bool usesDynamicOffset = false);

	private:
        
        struct SRGSlot
        {
            RHI::SRGType srgType{};
            int set;
        };
        
        Array<SRGSlot> srgSlots{};
        
        HashMap<RHI::SRGType, SRGSlot> builtinSrgNameToDescriptorSet{};
		HashMap<int, Array<SRGSlot>> setNumberToSrgs{};
		
		u32 maxBoundDescriptorSets = 0;

		

	};

	class ShaderResourceGroup : public RHI::ShaderResourceGroup
	{
	public:

		ShaderResourceGroup(VulkanDevice* device, const RHI::ShaderResourceGroupLayout& srgLayout);

		virtual ~ShaderResourceGroup();

		bool Bind(Name name, RHI::Buffer* buffer, SIZE_T offset = 0, SIZE_T size = 0) override;
		
		inline int GetSetNumber() const { return setNumber; }

		inline VkDescriptorSet GetDescriptorSet() const { return descriptorSet; }
		inline VkDescriptorSetLayout GetDescriptorSetLayout() const { return setLayout; }

		void Compile() override;

		inline bool IsFailed() const
		{
			return failed;
		}

	protected:

		ShaderResourceGroup(VulkanDevice* device);

		void Destroy();

		void CompileBindings();

		bool failed = false;

		VulkanDevice* device = nullptr;
		ShaderResourceManager* srgManager = nullptr;
		VulkanDescriptorPool* pool = null;
		VkDescriptorPool allocPool = null;

		int setNumber = -1;
		VkDescriptorSet descriptorSet = nullptr;
		VkDescriptorSetLayout setLayout = nullptr;

		Array<VkDescriptorSetLayoutBinding> setLayoutBindings{};
		HashMap<Name, int> bindingSlotsByVariableName{};
		
		HashMap<Name, VkDescriptorSetLayoutBinding> variableBindingsByName{};
		HashMap<int, VkDescriptorSetLayoutBinding> variableBindingsBySlot{};

		HashMap<int, VkDescriptorBufferInfo> bufferInfosBoundBySlot{};
		HashMap<int, VkDescriptorImageInfo> imageInfosBoundBySlot{};

		friend class VulkanGraphicsPipeline;
		friend class GraphicsCommandList;
        friend class VulkanDescriptorSet;
		friend class MergedShaderResourceGroup;
	};

} // namespace CE
