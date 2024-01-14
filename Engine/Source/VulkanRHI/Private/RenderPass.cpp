
#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
	RenderPass::RenderPass(VulkanDevice* device, const RenderPassDescriptor& desc)
		: device(device)
	{
		

		VkRenderPassCreateInfo renderPassCI{};
		renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCI.flags = 0;


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

} // namespace CE

