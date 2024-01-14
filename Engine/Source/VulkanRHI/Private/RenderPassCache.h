#pragma once

namespace CE::Vulkan
{

	class VULKANRHI_API RenderPassCache final
	{
	public:
		RenderPassCache(VulkanDevice* device);
		virtual ~RenderPassCache();

	private:
		VulkanDevice* device = nullptr;

	};
    
} // namespace CE::Vulkan
