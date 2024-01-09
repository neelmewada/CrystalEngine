#pragma once

namespace CE::Vulkan
{
    
	class VULKANRHI_API FrameGraphCompiler final : RHI::FrameGraphCompiler
	{
	public:
		FrameGraphCompiler(VulkanDevice* device);
		virtual ~FrameGraphCompiler();

	private:

		VulkanDevice* device = nullptr;
	};

} // namespace CE::Vulkan
