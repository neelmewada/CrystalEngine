#pragma once

namespace CE::Vulkan
{
	class VulkanDescriptorSet;
	class ShaderResourceGroup;
	class MergedShaderResourceGroup;

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

		MergedShaderResourceGroup* FindOrCreateMergedSRG(u32 srgCount, ShaderResourceGroup** srgs);

		MergedShaderResourceGroup* CreateMergedSRG(u32 srgCount, ShaderResourceGroup** srgs);
		void RemoveMergedSRG(MergedShaderResourceGroup* srg);
		void OnSRGDestroyed(ShaderResourceGroup* srg);

		void DestroyQueuedSRG();

	private:
        
        struct SRGSlot
        {
            RHI::SRGType srgType{};
            int set;
        };
        
		VulkanDevice* device = nullptr;
        Array<SRGSlot> srgSlots{};
        
        HashMap<RHI::SRGType, SRGSlot> srgTypeToDescriptorSet{};
		HashMap<int, Array<SRGSlot>> setNumberToSrgs{};
		
		u32 maxBoundDescriptorSets = 0;

		HashMap<SIZE_T, MergedShaderResourceGroup*> mergedSRGsByHash{};

		/// @brief HashMap of Merged SRG by each source SRG. Used to manage lifetime of Merged SRG.
		/// If any one of the source SRG that comprises the Merged SRG is destroyed, the Merged SRG should be destroyed.
		HashMap<Vulkan::ShaderResourceGroup*, Array<MergedShaderResourceGroup*>> mergedSRGsBySourceSRG{};

		Array<Vulkan::ShaderResourceGroup*> destroyQueue{};

		friend class ShaderResourceGroup;
		friend class MergedShaderResourceGroup;
	};

	class ShaderResourceGroup : public RHI::ShaderResourceGroup
	{
	public:

		ShaderResourceGroup(VulkanDevice* device, const RHI::ShaderResourceGroupLayout& srgLayout);

		virtual ~ShaderResourceGroup();

		virtual bool IsMerged() const { return false; }

		bool Bind(Name name, RHI::Buffer* buffer, SIZE_T offset = 0, SIZE_T size = 0) override;
		
		inline int GetSetNumber() const { return setNumber; }

		inline VkDescriptorSet GetDescriptorSet() const { return descriptorSet; }
		inline VkDescriptorSetLayout GetDescriptorSetLayout() const { return setLayout; }

		void Compile() override;

		inline bool IsFailed() const
		{
			return failed;
		}

		void QueueDestroy();

		virtual SIZE_T GetHash() const { return (SIZE_T)this; }

		inline void SetCommitted(bool set)
		{
			isCommitted = set;
		}

	protected:

		ShaderResourceGroup(VulkanDevice* device);

		void Destroy();

		void UpdateBindings();

		bool failed = false;

		VulkanDevice* device = nullptr;
		ShaderResourceManager* srgManager = nullptr;
		VulkanDescriptorPool* pool = nullptr;
		VkDescriptorPool allocPool = nullptr;

		int setNumber = -1;
		VkDescriptorSet descriptorSet = nullptr;
		VkDescriptorSetLayout setLayout = nullptr;

		Array<VkDescriptorSetLayoutBinding> setLayoutBindings{};
		HashMap<Name, int> bindingSlotsByVariableName{};
		
		HashMap<Name, VkDescriptorSetLayoutBinding> variableBindingsByName{};
		HashMap<int, VkDescriptorSetLayoutBinding> variableBindingsBySlot{};

		HashMap<int, VkDescriptorBufferInfo> bufferInfosBoundBySlot{};
		HashMap<int, VkDescriptorImageInfo> imageInfosBoundBySlot{};

		friend class GraphicsPipelineState;
		friend class CommandList;
        friend class VulkanDescriptorSet;
		friend class MergedShaderResourceGroup;
		friend class ShaderResourceManager;

	};

} // namespace CE
