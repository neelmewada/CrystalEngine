#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

	FrameBuffer::FrameBuffer(VulkanDevice* device, Scope* scope, u32 imageIndex)
		: device(device)
		, imageIndex(imageIndex)
	{
		VkFramebufferCreateInfo framebufferCI{};
		framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

		FixedArray<VkImageView, RHI::Limits::Pipeline::MaxRenderAttachmentCount> attachments{};

		width = 0;
		height = 0;

		Vulkan::Scope* current = scope;
		HashSet<AttachmentID> addedAttachments{};
		
		while (current != nullptr)
		{
			for (RHI::ScopeAttachment* attachment : current->attachments)
			{
				if (!attachment->IsImageAttachment())
					continue;
				auto frameAttachment = attachment->GetFrameAttachment();
				if (!frameAttachment)
					continue;
				if (addedAttachments.Exists(frameAttachment->GetId()))
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
				RHI::RHIResource* resource = imageScopeAttachment->GetFrameAttachment()->GetResource(imageIndex);
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
				addedAttachments.Add(frameAttachment->GetId());
			}

			current = (Vulkan::Scope*)current->nextSubPass;
		}

		framebufferCI.attachmentCount = attachments.GetSize();
		framebufferCI.pAttachments = attachments.GetData();

		framebufferCI.renderPass = scope->renderPass->GetHandle();
		framebufferCI.layers = 1;
		framebufferCI.width = width;
		framebufferCI.height = height;
		
		vkCreateFramebuffer(device->GetHandle(), &framebufferCI, nullptr, &frameBuffer);

		renderTarget = new Vulkan::RenderTarget(device, scope->renderPass);
	}

	FrameBuffer::~FrameBuffer()
	{
		if (frameBuffer)
		{
			vkDestroyFramebuffer(device->GetHandle(), frameBuffer, nullptr);
			frameBuffer = nullptr;
		}

		delete renderTarget; renderTarget = nullptr;
		
		device = nullptr;
	}

	FrameBuffer::FrameBuffer(VulkanDevice* device, const Array<Vulkan::Texture*>& images, RenderPass* renderPass, u32 imageIndex)
		: device(device), imageIndex(imageIndex)
	{
		FixedArray<VkImageView, RHI::Limits::Pipeline::MaxRenderAttachmentCount> attachments{};

		width = 0;
		height = 0;

		VkFramebufferCreateInfo framebufferCI{};
		framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

		for (Vulkan::Texture* image : images)
		{
			if (image != nullptr && image->GetImageView() != nullptr)
			{
				if (width == 0 || height == 0)
				{
					width = image->GetWidth();
					height = image->GetHeight();
				}
				else if (width != image->GetWidth() || height != image->GetHeight())
				{
					width = height = 0;
					CE_LOG(Error, All, "Failed to create vulkan framebuffer: Width and/or height mismatch!");
					return;
				}

				attachments.Add(image->GetImageView());
			}
		}

		framebufferCI.renderPass = renderPass->GetHandle();
		framebufferCI.layers = 1;
		framebufferCI.width = width;
		framebufferCI.height = height;

		framebufferCI.attachmentCount = attachments.GetSize();
		framebufferCI.pAttachments = attachments.GetData();

		vkCreateFramebuffer(device->GetHandle(), &framebufferCI, nullptr, &frameBuffer);

		renderTarget = new RenderTarget(device, renderPass);
	}

	FrameBuffer::FrameBuffer(VulkanDevice* device, const Array<Vulkan::TextureView*>& imageViews, RenderPass* renderPass, u32 imageIndex)
		: device(device), imageIndex(imageIndex)
	{
		FixedArray<VkImageView, RHI::Limits::Pipeline::MaxRenderAttachmentCount> attachments{};

		width = 0;
		height = 0;

		VkFramebufferCreateInfo framebufferCI{};
		framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

		for (Vulkan::TextureView* imageView : imageViews)
		{
			if (imageView != nullptr && imageView->GetImageView() != nullptr)
			{
				RHI::Texture* image = imageView->GetTexture();

				if (width == 0 || height == 0)
				{
					width = image->GetWidth();
					height = image->GetHeight();
				}
				else if (width != image->GetWidth() || height != image->GetHeight())
				{
					width = height = 0;
					CE_LOG(Error, All, "Failed to create vulkan framebuffer: Width and/or height mismatch!");
					return;
				}

				attachments.Add(imageView->GetImageView());
			}
		}

		framebufferCI.renderPass = renderPass->GetHandle();
		framebufferCI.layers = 1;
		framebufferCI.width = width;
		framebufferCI.height = height;

		framebufferCI.attachmentCount = attachments.GetSize();
		framebufferCI.pAttachments = attachments.GetData();

		vkCreateFramebuffer(device->GetHandle(), &framebufferCI, nullptr, &frameBuffer);

		renderTarget = new RenderTarget(device, renderPass);
	}

} // namespace CE::Vulkan
