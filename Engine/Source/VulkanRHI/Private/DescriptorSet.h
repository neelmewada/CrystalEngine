#pragma once

namespace CE::Vulkan
{
    class ShaderResourceManager;
    class CommandList;

    class DescriptorSet
    {
    public:
        DescriptorSet(VulkanDevice* device, const RHI::ShaderResourceGroupLayout& srgLayout);

        virtual ~DescriptorSet();

        inline VkDescriptorSet GetHandle() const { return descriptorSet; }

        inline VkDescriptorSetLayout GetSetLayout() const { return setLayout; }

    private:

        VulkanDevice* device = nullptr;

        RHI::ShaderResourceGroupLayout srgLayout{};
        int setNumber = -1;

        VkDescriptorSet descriptorSet = nullptr;
        VkDescriptorSetLayout setLayout = nullptr;
        VkDescriptorPool descriptorPool = nullptr;
        ShaderResourceManager* srgManager = nullptr;
        VulkanDescriptorPool* pool = nullptr;

        Array<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        HashMap<Name, int> bindingSlotsByVariableName{};

        HashMap<Name, VkDescriptorSetLayoutBinding> variableBindingsByName{};
        HashMap<int, VkDescriptorSetLayoutBinding> variableBindingsBySlot{};

        Atomic<int> usageCount = 0;

        bool failed = false;

        friend class ShaderResourceManager;
        friend class ShaderResourceGroup;
        friend class CE::Vulkan::CommandList;
    };
    
} // namespace CE::Vulkan
