#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    
	VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device, SwapChain* swapChain, u32 swapChainImageIndex, RenderTarget* renderTarget)
	{
		this->device = device;
		const auto& rtLayout = renderTarget->rtLayout;

		VkFramebufferCreateInfo frameBufferCI{};
		frameBufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

		frameBufferCI.attachmentCount = rtLayout.colorAttachmentCount + (rtLayout.HasDepthStencilAttachment() ? 1 : 0);

		List<VkImageView> attachments{ frameBufferCI.attachmentCount };
		attachments[rtLayout.presentationRTIndex] = swapChain->swapChainColorImages[swapChainImageIndex].imageView;

		if (rtLayout.HasDepthStencilAttachment())
		{
			attachments[attachments.GetSize() - 1] = swapChain->swapChainDepthImage->GetImageView();
		}

		frameBufferCI.pAttachments = attachments.GetData();

		frameBufferCI.width = swapChain->GetWidth();
		frameBufferCI.height = swapChain->GetHeight();

		frameBufferCI.renderPass = renderTarget->GetVulkanRenderPass()->GetHandle();

		frameBufferCI.layers = 1;

		if (vkCreateFramebuffer(device->GetHandle(), &frameBufferCI, nullptr, &frameBuffer) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan Frame Buffer");
			return;
		}
	}

	VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device,
		VkImageView attachments[RHI::MaxSimultaneousRenderOutputs + 1],
		RenderTarget* renderTarget)
	{
		this->device = device;
		const auto& rtLayout = renderTarget->rtLayout;

		VkFramebufferCreateInfo frameBufferCI{};
		frameBufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

		frameBufferCI.attachmentCount = rtLayout.colorAttachmentCount + (rtLayout.HasDepthStencilAttachment() ? 1 : 0);
		frameBufferCI.pAttachments = attachments;

		frameBufferCI.width = renderTarget->GetWidth();
		frameBufferCI.height = renderTarget->GetHeight();

		frameBufferCI.renderPass = renderTarget->GetVulkanRenderPass()->GetHandle();

		frameBufferCI.layers = 1;

		if (vkCreateFramebuffer(device->GetHandle(), &frameBufferCI, nullptr, &frameBuffer) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan Frame Buffer");
			return;
		}
	}

	VulkanFrameBuffer::~VulkanFrameBuffer()
	{
		vkDestroyFramebuffer(device->GetHandle(), frameBuffer, nullptr);
	}

} // namespace CE::Vulkan
