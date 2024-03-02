#pragma once

namespace CE::Vulkan
{
    class ShaderResourceManager;
    class CommandList;

    class DescriptorSet
    {
    public:
        DescriptorSet(VulkanDevice* device, VkDescriptorSetLayout setLayout);

        virtual ~DescriptorSet();

        inline VkDescriptorSet GetHandle() const { return descriptorSet; }

    private:

        VulkanDevice* device = nullptr;

        RHI::ShaderResourceGroupLayout srgLayout{};
        int setNumber = -1;

        VkDescriptorSet descriptorSet = nullptr;
        VkDescriptorPool descriptorPool = nullptr;
        ShaderResourceManager* srgManager = nullptr;
        DescriptorPool* pool = nullptr;

        Atomic<int> usageCount = 0;

        bool failed = false;

        friend class ShaderResourceManager;
        friend class ShaderResourceGroup;
        friend class CE::Vulkan::CommandList;
    };
    
} // namespace CE::Vulkan
