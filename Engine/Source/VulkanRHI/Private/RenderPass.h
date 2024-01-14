#pragma once

#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
	struct RenderPassDescriptor
	{
		struct SubPassDescriptor
		{
			FixedArray<VkAttachmentReference, RHI::Limits::Pipeline::MaxColorAttachmentCount> colorAttachmentRefs{};
			FixedArray<VkAttachmentReference, 1> depthAttachmentRef{};
			FixedArray<VkAttachmentReference, RHI::Limits::Pipeline::MaxColorAttachmentCount> inputAttachmentRefs{};
		};

		FixedArray<VkAttachmentDescription, RHI::Limits::Pipeline::MaxRenderAttachmentCount> attachments{};
		FixedArray<SubPassDescriptor, RHI::Limits::Pipeline::MaxSubPassCount> subpasses{};
	};

	class RenderPass final
	{
	public:

		RenderPass(VulkanDevice* device, const RenderPassDescriptor& desc);
		virtual ~RenderPass();
		
	private:
		VkRenderPass renderPass = nullptr;
		VulkanDevice* device = nullptr;

	};

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
