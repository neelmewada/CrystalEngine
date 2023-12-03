
#include "VulkanRHI.h"
#include "VulkanRHIPrivate.h"

#include "VulkanDescriptorPool.h"

namespace CE
{
    
    VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice* device, u32 initialPoolSize, u32 poolSizeIncrement)
        : device(device), initialSize(initialPoolSize), incrementSize(poolSizeIncrement)
    {
		VkDescriptorPoolSize poolSizes[] = {
			{.type = VK_DESCRIPTOR_TYPE_SAMPLER, .descriptorCount = initialPoolSize },
			{.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, .descriptorCount = initialPoolSize },
			{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = initialPoolSize },
			{.type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, .descriptorCount = initialPoolSize },
			{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, .descriptorCount = initialPoolSize },
			{.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .descriptorCount = initialPoolSize },
			{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = initialPoolSize },
			{.type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, .descriptorCount = initialPoolSize },
			{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, .descriptorCount = initialPoolSize },
			{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = initialPoolSize }
		};
        
        VkDescriptorPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.maxSets = initialPoolSize * 2;
        info.poolSizeCount = COUNTOF(poolSizes);
        info.pPoolSizes = poolSizes;
		info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;// | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        
        VkDescriptorPool pool = nullptr;
        
        auto result = vkCreateDescriptorPool(device->GetHandle(), &info, nullptr, &pool);
        if (result != VK_SUCCESS)
        {
            CE_LOG(Error, All, "Failed to create vulkan descriptor pool. Error code: {}", (int)result);
            return;
        }
        
        descriptorPools.Add(pool);
    }

    VulkanDescriptorPool::~VulkanDescriptorPool()
    {
        for (VkDescriptorPool pool : descriptorPools)
        {
            vkDestroyDescriptorPool(device->GetHandle(), pool, nullptr);
        }
        descriptorPools.Clear();
    }

	List<VkDescriptorSet> VulkanDescriptorPool::Allocate(u32 numDescriptorSets, List<VkDescriptorSetLayout> setLayouts, VkDescriptorPool& outPool)
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorSetCount = numDescriptorSets;
		allocInfo.pSetLayouts = setLayouts.GetData();

		if (descriptorPools.IsEmpty())
			Increment(Math::Max(incrementSize, numDescriptorSets));
		allocInfo.descriptorPool = descriptorPools.GetLast();

		List<VkDescriptorSet> sets = List<VkDescriptorSet>(numDescriptorSets);

		VkResult result = vkAllocateDescriptorSets(device->GetHandle(), &allocInfo, sets.GetData());
		int counter = 0;

		if (result == VK_ERROR_OUT_OF_POOL_MEMORY)
		{
			// If we need a lot of descriptor sets allocated
			if (numDescriptorSets > initialSize)
			{
				Increment(Math::Max(incrementSize, numDescriptorSets));
				allocInfo.descriptorPool = descriptorPools.GetLast();
				result = vkAllocateDescriptorSets(device->GetHandle(), &allocInfo, sets.GetData());
			}

			for (int i = descriptorPools.GetSize() - 1; i >= 0 && result != VK_SUCCESS; i--)
			{
				allocInfo.descriptorPool = descriptorPools[i];
				result = vkAllocateDescriptorSets(device->GetHandle(), &allocInfo, sets.GetData());
			}

			if (result != VK_SUCCESS)
			{
				Increment(Math::Max(incrementSize, numDescriptorSets));
			}

			for (int i = descriptorPools.GetSize() - 1; i >= 0 && result != VK_SUCCESS; i--)
			{
				allocInfo.descriptorPool = descriptorPools[i];
				result = vkAllocateDescriptorSets(device->GetHandle(), &allocInfo, sets.GetData());
			}
		}

		if (result != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to allocate descriptor sets! Error code {}", (int)result);
			return {};
		}
		
		outPool = allocInfo.descriptorPool;
		return sets;
	}

	void VulkanDescriptorPool::Free(const List<VkDescriptorSet>& sets)
	{
		if (sets.IsEmpty())
			return;

		for (int i = descriptorPools.GetSize() - 1; i >= 0; i--)
		{
			auto result = vkFreeDescriptorSets(device->GetHandle(), descriptorPools[i], sets.GetSize(), sets.GetData());

			if (result == VK_SUCCESS)
				return;
		}
	}

    void VulkanDescriptorPool::Increment(u32 incrementSize)
    {
        VkDescriptorPoolSize poolSizes[] = {
            { .type = VK_DESCRIPTOR_TYPE_SAMPLER, .descriptorCount = incrementSize },
            { .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, .descriptorCount = incrementSize },
            { .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = incrementSize },
            { .type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, .descriptorCount = incrementSize },
            { .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, .descriptorCount = incrementSize },
            { .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .descriptorCount = incrementSize },
			{ .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = incrementSize },
			{ .type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, .descriptorCount = incrementSize },
			{ .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, .descriptorCount = incrementSize },
            { .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = incrementSize }
        };
        
        VkDescriptorPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.maxSets = incrementSize * 2;
        info.poolSizeCount = COUNTOF(poolSizes);
        info.pPoolSizes = poolSizes;
		info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;// | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        
        VkDescriptorPool pool = nullptr;
        
        auto result = vkCreateDescriptorPool(device->GetHandle(), &info, nullptr, &pool);
        if (result != VK_SUCCESS)
        {
            CE_LOG(Error, All, "Failed to create vulkan descriptor pool. Error code: {}", (int)result);
            return;
        }
        
        descriptorPools.Add(pool);
    }

} // namespace CE

