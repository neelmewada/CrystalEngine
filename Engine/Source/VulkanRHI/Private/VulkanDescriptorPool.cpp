
#include "VulkanRHI.h"
#include "VulkanRHIPrivate.h"

#include "VulkanDescriptorPool.h"

namespace CE
{
    
    VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice* device, u32 initialPoolSize, u32 poolSizeIncrement)
        : device(device), initialSize(initialPoolSize), incrementSize(poolSizeIncrement)
    {
        VkDescriptorPoolSize poolSizes[] = {
            { .type = VK_DESCRIPTOR_TYPE_SAMPLER, .descriptorCount = initialPoolSize },
            { .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, .descriptorCount = initialPoolSize },
            { .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = initialPoolSize },
            { .type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, .descriptorCount = initialPoolSize },
            { .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, .descriptorCount = initialPoolSize },
            { .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .descriptorCount = initialPoolSize },
            { .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = initialPoolSize },
            { .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = initialPoolSize }
        };
        
        VkDescriptorPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.maxSets = initialPoolSize;
        info.poolSizeCount = COUNTOF(poolSizes);
        info.pPoolSizes = poolSizes;
        
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

    void VulkanDescriptorPool::Increment()
    {
        VkDescriptorPoolSize poolSizes[] = {
            { .type = VK_DESCRIPTOR_TYPE_SAMPLER, .descriptorCount = incrementSize },
            { .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, .descriptorCount = incrementSize },
            { .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = incrementSize },
            { .type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, .descriptorCount = incrementSize },
            { .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, .descriptorCount = incrementSize },
            { .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .descriptorCount = incrementSize },
            { .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = incrementSize },
            { .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = incrementSize }
        };
        
        VkDescriptorPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.maxSets = incrementSize;
        info.poolSizeCount = COUNTOF(poolSizes);
        info.pPoolSizes = poolSizes;
        info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        
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

