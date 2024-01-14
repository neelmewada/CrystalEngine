
#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

	RenderPass::RenderPass(VulkanDevice* device, const RenderPass::Descriptor& descriptor)
		: device(device)
		, desc(descriptor)
	{


		VkRenderPassCreateInfo renderPassCI{};
		renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCI.flags = 0;
		
		FixedArray<VkAttachmentDescription, RHI::Limits::Pipeline::MaxRenderAttachmentCount> attachments{};
		
		for (const RenderPass::AttachmentBinding& attachmentBinding : desc.attachments)
		{
			VkAttachmentDescription attachmentDesc{};
			attachmentDesc.initialLayout = attachmentBinding.initialLayout;
			attachmentDesc.finalLayout = attachmentDesc.finalLayout;
			attachmentDesc.format = RHIFormatToVkFormat(attachmentBinding.format);
			attachmentDesc.loadOp = RHIAttachmentLoadActionToVk(attachmentBinding.loadStoreAction.loadAction);
			attachmentDesc.storeOp = RHIAttachmentStoreActionToVk(attachmentBinding.loadStoreAction.storeAction);
			attachmentDesc.stencilLoadOp = RHIAttachmentLoadActionToVk(attachmentBinding.loadStoreAction.loadActionStencil);
			attachmentDesc.stencilStoreOp = RHIAttachmentStoreActionToVk(attachmentBinding.loadStoreAction.storeActionStencil);
			attachmentDesc.samples = GetSampleCountFlagBits(attachmentBinding.multisampleState.sampleCount);
			
			attachments.Add(attachmentDesc);
		}
		
		renderPassCI.attachmentCount = attachments.GetSize();
		renderPassCI.pAttachments = attachments.GetData();

		FixedArray<VkSubpassDescription, RHI::Limits::Pipeline::MaxSubPassCount> subpasses{};

		for (const RenderPass::SubPassDescriptor& subpassDesc : desc.subpasses)
		{
			VkSubpassDescription subpass{};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			
			FixedArray<VkAttachmentReference, RHI::Limits::Pipeline::MaxColorAttachmentCount> colorAttachments{};
			for (const RenderPass::SubPassAttachment& subpassAttachment : subpassDesc.renderTargetAttachments)
			{
				VkAttachmentReference ref{};
				ref.attachment = subpassAttachment.attachmentIndex;
				ref.layout = subpassAttachment.imageLayout;
				colorAttachments.Add(ref);
			}

			subpass.colorAttachmentCount = colorAttachments.GetSize();
			subpass.pColorAttachments = colorAttachments.GetData();

			FixedArray<VkAttachmentReference, RHI::Limits::Pipeline::MaxColorAttachmentCount> resolveAttachments{};
			for (const RenderPass::SubPassAttachment& subpassAttachment : subpassDesc.resolveAttachments)
			{
				VkAttachmentReference ref{};
				ref.attachment = subpassAttachment.attachmentIndex;
				ref.layout = subpassAttachment.imageLayout;
				resolveAttachments.Add(ref);
			}

			if (!resolveAttachments.IsEmpty())
				subpass.pResolveAttachments = resolveAttachments.GetData();

			FixedArray<VkAttachmentReference, RHI::Limits::Pipeline::MaxColorAttachmentCount> subpassInputAttachments{};
			for (const RenderPass::SubPassAttachment& subpassAttachment : subpassDesc.subpassInputAttachments)
			{
				VkAttachmentReference ref{};
				ref.attachment = subpassAttachment.attachmentIndex;
				ref.layout = subpassAttachment.imageLayout;
				subpassInputAttachments.Add(ref);
			}

			subpass.inputAttachmentCount = subpassInputAttachments.GetSize();
			subpass.pInputAttachments = subpassInputAttachments.GetData();

			FixedArray<u32, RHI::Limits::Pipeline::MaxColorAttachmentCount> preserveAttachments{};
			for (u32 index : subpassDesc.preserveAttachments)
			{
				preserveAttachments.Add(index);
			}

			subpass.preserveAttachmentCount = preserveAttachments.GetSize();
			subpass.pPreserveAttachments = preserveAttachments.GetData();

			subpass.pDepthStencilAttachment = nullptr;
			if (subpassDesc.depthStencilAttachment.GetSize() > 0)
			{
				VkAttachmentReference ref{};
				ref.attachment = subpassDesc.depthStencilAttachment[0].attachmentIndex;
				ref.layout = subpassDesc.depthStencilAttachment[0].imageLayout;
			}

			subpasses.Add(subpass);
		}
	}

	RenderPass::~RenderPass()
	{
		if (renderPass != nullptr)
		{
			vkDestroyRenderPass(device->GetHandle(), renderPass, nullptr);
		}
	}
    
    /*class VulkanRenderPassBuilder
    {
    public:
        VulkanRenderPassBuilder(VulkanDevice* device) : device(device)
        {

        }

        VkRenderPass Build(const VulkanRenderTargetLayout& rtLayout)
        {
            u32 subpassCount = 0;
            u32 dependencyCount = 0;

            // - Forward Rendering -

            // Main Subpass 0
            {
                VkSubpassDescription& desc = subpasses[subpassCount++];
                desc.flags = 0;

                desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

                desc.preserveAttachmentCount = 0;
                desc.pPreserveAttachments = nullptr;

                desc.inputAttachmentCount = 0;
                desc.pInputAttachments = nullptr;

                desc.colorAttachmentCount = rtLayout.colorAttachmentCount;
                desc.pColorAttachments = rtLayout.colorReferences;
				desc.pResolveAttachments = nullptr; // Count = colorAttachmentCount

                if (rtLayout.HasDepthStencilAttachment())
                    desc.pDepthStencilAttachment = &rtLayout.depthStencilReference;
                else
                    desc.pDepthStencilAttachment = nullptr;
            }

            // Dependency EXTERNAL -> Subpass 0
            {
                VkSubpassDependency& dependency = dependencies[dependencyCount++];
                
                dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
                dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.srcAccessMask = 0;

                dependency.dstSubpass = 0;
                dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				
                dependency.dependencyFlags = 0;
            }

            VkRenderPassCreateInfo renderPassCI{};
            renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassCI.flags = 0;

            renderPassCI.subpassCount = subpassCount;
            renderPassCI.pSubpasses = subpasses;

            renderPassCI.dependencyCount = dependencyCount;
            renderPassCI.pDependencies = dependencies;

            renderPassCI.attachmentCount = rtLayout.attachmentDescCount;
            renderPassCI.pAttachments = rtLayout.attachmentDesc;

            VkRenderPass renderPass = nullptr;
            if (vkCreateRenderPass(device->GetHandle(), &renderPassCI, nullptr, &renderPass) != VK_SUCCESS)
            {
                CE_LOG(Error, All, "Failed to create vulkan render pass.");
                return nullptr;
            }

            return renderPass;
        }

    private:
        VulkanDevice* device = nullptr;
		VkSubpassDescription subpasses[RHI::Limits::Pipeline::MaxSubPassCount] = {};
		VkSubpassDependency dependencies[RHI::Limits::Pipeline::MaxSubPassCount + 2] = {};
    };*/

	VkSampleCountFlagBits GetSampleCountFlagBits(int sampleCount)
	{
		switch (sampleCount)
		{
		case 1:
			return VK_SAMPLE_COUNT_1_BIT;
		case 2:
			return VK_SAMPLE_COUNT_2_BIT;
		case 4:
			return VK_SAMPLE_COUNT_4_BIT;
		case 8:
			return VK_SAMPLE_COUNT_8_BIT;
		case 16:
			return VK_SAMPLE_COUNT_16_BIT;
		case 32:
			return VK_SAMPLE_COUNT_32_BIT;
		case 64:
			return VK_SAMPLE_COUNT_64_BIT;
		}
		return VK_SAMPLE_COUNT_1_BIT;
	}

	VkAttachmentLoadOp RHIAttachmentLoadActionToVk(RHI::AttachmentLoadAction loadAction)
	{
		switch (loadAction)
		{
		case RHI::AttachmentLoadAction::Clear:
			return VK_ATTACHMENT_LOAD_OP_CLEAR;
		case RHI::AttachmentLoadAction::Load:
			return VK_ATTACHMENT_LOAD_OP_LOAD;
		case RHI::AttachmentLoadAction::DontCare:
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		}
		return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	}

	VkAttachmentStoreOp RHIAttachmentStoreActionToVk(RHI::AttachmentStoreAction storeAction)
	{
		switch (storeAction)
		{
		case RHI::AttachmentStoreAction::DontCare:
			return VK_ATTACHMENT_STORE_OP_NONE;
		case RHI::AttachmentStoreAction::Store:
			return VK_ATTACHMENT_STORE_OP_STORE;
		}
		return VK_ATTACHMENT_STORE_OP_DONT_CARE;
	}

} // namespace CE

