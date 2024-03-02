#pragma once

namespace CE::Vulkan
{

    class DescriptorPool
    {
    public:
        
        DescriptorPool(VulkanDevice* device, u32 initialPoolSize = 128, u32 poolSizeIncrement = 128);
        
        ~DescriptorPool();
        
		List<VkDescriptorSet> Allocate(u32 numDescriptorSets, List<VkDescriptorSetLayout> setLayouts, VkDescriptorPool& outPool);

		void Free(const List<VkDescriptorSet>& sets);
        
        void Increment(u32 size);
        
    private:
        
        VulkanDevice* device = nullptr;
        List<VkDescriptorPool> descriptorPools{};
        
        u32 initialSize = 128;
        u32 incrementSize = 128;
        
    };

} // namespace CE
