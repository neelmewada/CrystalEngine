#pragma once

namespace CE::Vulkan
{

	class VULKANRHI_API RenderPassCache final
	{
	public:
		RenderPassCache(VulkanDevice* device);
		virtual ~RenderPassCache();

		RenderPass* FindOrCreate(const RenderPass::Descriptor& desc);

	private:
		VulkanDevice* device = nullptr;

		SharedMutex mutex{};
		HashMap<SIZE_T, RenderPass*> renderPassCache{};
	};
    
} // namespace CE::Vulkan
