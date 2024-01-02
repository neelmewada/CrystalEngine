#pragma once

namespace CE::Vulkan
{

	class VulkanSwapChain;
	class RenderTarget;
	class Texture;

	class VulkanFrameBuffer
	{
	public:
		VulkanFrameBuffer(VulkanDevice* device, VulkanSwapChain* swapChain, u32 swapChainImageIndex, RenderTarget* renderTarget);
		VulkanFrameBuffer(VulkanDevice* device, VkImageView attachments[RHI::MaxSimultaneousRenderOutputs + 1], RenderTarget* renderTarget);

		~VulkanFrameBuffer();

		inline VkFramebuffer GetHandle() const
		{
			return frameBuffer;
		}

	private:
		VulkanDevice* device = nullptr;
		VkFramebuffer frameBuffer = nullptr;
	};
    
} // namespace CE::Vulkan
