#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    DescriptorSet::DescriptorSet(VulkanDevice* device, VkDescriptorSetLayout setLayout)
		: device(device)
    {
		this->srgLayout = srgLayout;
		this->srgManager = device->GetShaderResourceManager();
		pool = device->GetDescriptorPool();
		setNumber = srgManager->GetDescriptorSetNumber(srgLayout.srgType);

		auto allocatedSets = pool->Allocate(1, { setLayout }, descriptorPool);
		if (allocatedSets.IsEmpty())
		{
			failed = true;
			CE_LOG(Error, All, "Failed to allocate descriptor set");
			return;
		}

		descriptorSet = allocatedSets[0];
    }

    DescriptorSet::~DescriptorSet()
    {
		vkFreeDescriptorSets(device->GetHandle(), descriptorPool, 1, &descriptorSet);
		descriptorSet = nullptr;
    }

} // namespace CE::Vulkan
