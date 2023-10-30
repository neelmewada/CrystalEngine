#pragma once

namespace CE
{

    class VulkanDescriptorPool
    {
    public:
        
        VulkanDescriptorPool(VulkanDevice* device, u32 initialPoolSize = 128, u32 poolSizeIncrement = 128);
        
        ~VulkanDescriptorPool();
        
		List<VkDescriptorSet> Allocate(u32 numDescriptorSets, List<VkDescriptorSetLayout> setLayouts, VkDescriptorPool& outPool);

		void Free(const List<VkDescriptorSet>& sets);
        
        void Increment(int size);
        
    private:
        
        VulkanDevice* device = nullptr;
        List<VkDescriptorPool> descriptorPools{};
        
        u32 initialSize = 128;
        u32 incrementSize = 128;
        
    };

} // namespace CE
