#pragma once

#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

	class RenderPass final
	{
	public:

		struct AttachmentBinding
		{
			RHI::Format format = RHI::Format::Undefined;
			RHI::AttachmentLoadStoreAction loadStoreAction{};
			VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			VkImageLayout finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			RHI::MultisampleState multisampleState{};
		};

		struct SubPassAttachment
		{
			u32 attachmentIndex = u32(-1);
			VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			inline bool IsValid() const { return attachmentIndex != u32(-1); }
		};

		struct SubPassDescriptor
		{
			FixedArray<SubPassAttachment, RHI::Limits::Pipeline::MaxColorAttachmentCount> renderTargetAttachments{};
			FixedArray<SubPassAttachment, RHI::Limits::Pipeline::MaxColorAttachmentCount> resolveAttachments{};
			FixedArray<SubPassAttachment, RHI::Limits::Pipeline::MaxColorAttachmentCount> subpassInputAttachments{};
			FixedArray<u32, RHI::Limits::Pipeline::MaxColorAttachmentCount> preserveAttachments{};
			FixedArray<SubPassAttachment, 1> depthStencilAttachment{};
		};

		struct Descriptor
		{
			FixedArray<AttachmentBinding, RHI::Limits::Pipeline::MaxRenderAttachmentCount> attachments{};
			FixedArray<SubPassDescriptor, RHI::Limits::Pipeline::MaxSubPassCount> subpasses{};
			Array<VkSubpassDependency> dependencies{};
		};

		RenderPass(VulkanDevice* device, const Descriptor& desc);
		virtual ~RenderPass();
		
	private:
		VkRenderPass renderPass = nullptr;
		VulkanDevice* device = nullptr;

		Descriptor desc{};
	};

	VkSampleCountFlagBits GetSampleCountFlagBits(int sampleCount);
	VkAttachmentLoadOp RHIAttachmentLoadActionToVk(RHI::AttachmentLoadAction loadAction);
	VkAttachmentStoreOp RHIAttachmentStoreActionToVk(RHI::AttachmentStoreAction storeAction);

    /*class VulkanRenderPass
    {
    public:
        VulkanRenderPass(VulkanDevice* device, const VulkanRenderTargetLayout& rtLayout);
        ~VulkanRenderPass();

        CE_INLINE VkRenderPass GetHandle()
        {
            return renderPass;
        }

    private:
        VkRenderPass renderPass = nullptr;
        VulkanDevice* device = nullptr;
        VulkanRenderTargetLayout layout{};
    };*/

} // namespace CE
