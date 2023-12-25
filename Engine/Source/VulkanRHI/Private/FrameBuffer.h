#pragma once

namespace CE::Vulkan
{

	class VulkanSwapChain;
	class VulkanRenderTarget;
	class VulkanTexture;

	class VulkanFrameBuffer
	{
	public:
		VulkanFrameBuffer(VulkanDevice* device, VulkanSwapChain* swapChain, u32 swapChainImageIndex, VulkanRenderTarget* renderTarget);
		VulkanFrameBuffer(VulkanDevice* device, VkImageView attachments[RHI::MaxSimultaneousRenderOutputs + 1], VulkanRenderTarget* renderTarget);

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
