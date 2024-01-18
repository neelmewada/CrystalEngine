#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    
	VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device, SwapChain* swapChain, u32 swapChainImageIndex, RenderTarget* renderTarget)
	{
		this->device = device;
		/*const auto& rtLayout = renderTarget->rtLayout;

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
		}*/
	}

	VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device,
		VkImageView attachments[RHI::MaxSimultaneousRenderOutputs + 1],
		RenderTarget* renderTarget)
	{
		this->device = device;
		/*const auto& rtLayout = renderTarget->rtLayout;

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
		}*/
	}

	VulkanFrameBuffer::~VulkanFrameBuffer()
	{
		vkDestroyFramebuffer(device->GetHandle(), frameBuffer, nullptr);
	}

	FrameBuffer::FrameBuffer(VulkanDevice* device, Scope* scope, u32 imageIndex)
		: device(device)
		, imageIndex(imageIndex)
	{
		VkFramebufferCreateInfo framebufferCI{};
		framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

		FixedArray<VkImageView, RHI::Limits::Pipeline::MaxRenderAttachmentCount> attachments{};

		width = 0; 
		height = 0;
		
		for (RHI::ScopeAttachment* attachment : scope->attachments)
		{
			if (!attachment->IsImageAttachment())
				continue;
			switch (attachment->GetUsage())
			{
			case RHI::ScopeAttachmentUsage::None:
			case RHI::ScopeAttachmentUsage::Copy:
			case RHI::ScopeAttachmentUsage::Shader:
			case RHI::ScopeAttachmentUsage::COUNT:
				continue;
			}

			ImageScopeAttachment* imageScopeAttachment = (ImageScopeAttachment*)attachment;
			RHIResource* resource = imageScopeAttachment->GetFrameAttachment()->GetResource(imageIndex);
			if (!resource)
				continue;

			Texture* image = dynamic_cast<Texture*>(resource);
			if (!image || image->GetImageView() == nullptr)
				continue;

			if (width == 0 || height == 0)
			{
				width = image->GetWidth();
				height = image->GetHeight();
			}
			else
			{
				if (width != image->GetWidth() || height != image->GetHeight())
				{
					width = height = 0;
					CE_LOG(Error, All, "Failed to create vulkan framebuffer: Width or height mismatch!");
					return;
				}
			}

			attachments.Add(image->GetImageView());
		}

		framebufferCI.attachmentCount = attachments.GetSize();
		framebufferCI.pAttachments = attachments.GetData();

		framebufferCI.renderPass = scope->renderPass->GetHandle();
		framebufferCI.layers = 1;
		framebufferCI.width = width;
		framebufferCI.height = height;
		
		vkCreateFramebuffer(device->GetHandle(), &framebufferCI, nullptr, &frameBuffer);
	}

	FrameBuffer::~FrameBuffer()
	{
		vkDestroyFramebuffer(device->GetHandle(), frameBuffer, nullptr);
		frameBuffer = nullptr;
		device = nullptr;
	}

} // namespace CE::Vulkan
