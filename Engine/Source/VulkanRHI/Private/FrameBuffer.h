#pragma once

namespace CE::Vulkan
{

	class SwapChain;
	class RenderTarget;
	class Texture;

	class FrameBuffer
	{
	public:

		FrameBuffer(VulkanDevice* device, Scope* scope, u32 imageIndex);
		virtual ~FrameBuffer();

		inline VkFramebuffer GetHandle() const { return frameBuffer; }

		inline u32 GetWidth() const { return width; }
		inline u32 GetHeight() const { return height; }

	private:

		VulkanDevice* device = nullptr;
		VkFramebuffer frameBuffer = nullptr;

		u32 width = 0;
		u32 height = 0;
		u32 imageIndex = 0;
	};

	class VulkanFrameBuffer
	{
	public:
		VulkanFrameBuffer(VulkanDevice* device, SwapChain* swapChain, u32 swapChainImageIndex, RenderTarget* renderTarget);
		VulkanFrameBuffer(VulkanDevice* device, VkImageView attachments[RHI::MaxSimultaneousRenderOutputs + 1], RenderTarget* renderTarget);

		~VulkanFrameBuffer();

		

	private:
		VulkanDevice* device = nullptr;
		VkFramebuffer frameBuffer = nullptr;

		
	};
    
} // namespace CE::Vulkan
