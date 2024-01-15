#pragma once

#include "VulkanRHIPrivate.h"

namespace CE
{
	template<>
	inline SIZE_T GetHash<VkSubpassDependency>(const VkSubpassDependency& value)
	{
		SIZE_T hash = GetHash(value.srcSubpass);
		CombineHash(hash, value.dstSubpass);
		CombineHash(hash, value.srcStageMask);
		CombineHash(hash, value.dstStageMask);
		CombineHash(hash, value.srcAccessMask);
		CombineHash(hash, value.dstAccessMask);
		CombineHash(hash, value.dependencyFlags);
		return hash;
	}
}

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

			SIZE_T GetHash() const;
		};

		struct SubPassAttachment
		{
			u32 attachmentIndex = u32(-1);
			VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			inline bool IsValid() const { return attachmentIndex != u32(-1); }

			SIZE_T GetHash() const;
		};

		struct SubPassDescriptor
		{
			FixedArray<SubPassAttachment, RHI::Limits::Pipeline::MaxColorAttachmentCount> renderTargetAttachments{};
			FixedArray<SubPassAttachment, RHI::Limits::Pipeline::MaxColorAttachmentCount> resolveAttachments{};
			FixedArray<SubPassAttachment, RHI::Limits::Pipeline::MaxColorAttachmentCount> subpassInputAttachments{};
			FixedArray<u32, RHI::Limits::Pipeline::MaxColorAttachmentCount> preserveAttachments{};
			FixedArray<SubPassAttachment, 1> depthStencilAttachment{};

			SIZE_T GetHash() const;
		};

		struct Descriptor
		{
			FixedArray<AttachmentBinding, RHI::Limits::Pipeline::MaxRenderAttachmentCount> attachments{};
			FixedArray<SubPassDescriptor, RHI::Limits::Pipeline::MaxSubPassCount> subpasses{};
			Array<VkSubpassDependency> dependencies{};

			SIZE_T GetHash() const;
		};

		RenderPass(VulkanDevice* device, const Descriptor& desc);
		virtual ~RenderPass();

		static void BuildDescriptor(Scope* pass, Descriptor& outDescriptor);

		inline const Descriptor& GetDescriptor() const { return desc; }
		
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
