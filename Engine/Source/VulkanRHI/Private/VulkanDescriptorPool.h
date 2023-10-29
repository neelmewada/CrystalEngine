#pragma once

namespace CE
{

    class VulkanDescriptorPool
    {
    public:
        
        VulkanDescriptorPool(VulkanDevice* device, u32 initialPoolSize, u32 poolSizeIncrement);
        
        ~VulkanDescriptorPool();
        
        inline VkDescriptorPool GetDescriptorPool() const { return descriptorPool; }
        
        void Increment();
        
    private:
        
        VulkanDevice* device = nullptr;
        List<VkDescriptorPool> descriptorPools{};
        
        u32 initialSize = 128;
        u32 incrementSize = 128;
        
    };

} // namespace CE
