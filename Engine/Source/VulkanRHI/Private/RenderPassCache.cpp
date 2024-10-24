#include "VulkanRHIPrivate.h"


namespace CE::Vulkan
{

	RenderPassCache::RenderPassCache(VulkanDevice* device) : device(device)
    {

    }

    RenderPassCache::~RenderPassCache()
    {
		LockGuard<SharedMutex> lock{ mutex };

		for (auto [hash, renderPass] : renderPassCache)
		{
			delete renderPass;
		}
		renderPassCache.Clear();
    }

	RenderPass* RenderPassCache::FindOrCreate(const RenderPass::Descriptor& desc)
	{
		LockGuard<SharedMutex> lock{ mutex };

		SIZE_T hash = desc.GetHash();
		if (hash == 0)
			return nullptr;
		if (renderPassCache[hash] != nullptr)
			return renderPassCache[hash];

		RenderPass* renderPass = new RenderPass(device, desc);
		renderPass->hash = hash;
		renderPassCache[hash] = renderPass;
		return renderPass;
	}

} // namespace CE::Vulkan
