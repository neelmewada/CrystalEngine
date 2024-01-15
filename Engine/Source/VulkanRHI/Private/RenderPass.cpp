
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

		renderPassCI.subpassCount = subpasses.GetSize();
		renderPassCI.pSubpasses = subpasses.GetData();

		renderPassCI.dependencyCount = desc.dependencies.GetSize();
		if (desc.dependencies.NonEmpty())
			renderPassCI.pDependencies = desc.dependencies.GetData();

		auto result = vkCreateRenderPass(device->GetHandle(), &renderPassCI, nullptr, &renderPass);
		if (result != VK_SUCCESS)
		{
			return;
		}
	}

	RenderPass::~RenderPass()
	{
		if (renderPass != nullptr)
		{
			vkDestroyRenderPass(device->GetHandle(), renderPass, nullptr);
			renderPass = nullptr;
		}
	}

	void RenderPass::BuildDescriptor(Scope* pass, Descriptor& outDescriptor)
	{
		outDescriptor = {};
		if (pass == nullptr)
			return;

		// Multiple subpasses
		if (pass->nextSubPass != nullptr && pass->prevSubPass == nullptr)
		{
			Scope* cur = pass;
			
			while (cur != nullptr)
			{

				cur = (Vulkan::Scope*)cur->nextSubPass;
			}
		}
		else // Single subpass
		{
			HashMap<AttachmentID, FrameAttachment*> frameAttachments{};
			
			for (RHI::ScopeAttachment* attachment : pass->attachments)
			{
				auto frameAttachment = attachment->GetFrameAttachment();
				if (!frameAttachment->IsImageAttachment())
					continue;
				if (attachment->GetUsage() == RHI::ScopeAttachmentUsage::Shader ||
					attachment->GetUsage() == RHI::ScopeAttachmentUsage::Copy)
					continue;
				if (!frameAttachments.KeyExists(frameAttachment->GetId()))
				{
					frameAttachments[frameAttachment->GetId()] = frameAttachment;
				}
			}

			outDescriptor.attachments.Clear();
			HashMap<AttachmentID, AttachmentBinding*> attachmentBindingsById{};

			VkSubpassDependency dependency{};
			dependency.srcStageMask = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			outDescriptor.dependencies.Add(dependency);

			SubPassDescriptor subpass{};
			int i = 0;

			for (auto scopeAttachment : pass->attachments)
			{
				if (!scopeAttachment->IsImageAttachment() || scopeAttachment->GetFrameAttachment() == nullptr ||
					!scopeAttachment->GetFrameAttachment()->IsImageAttachment())
					continue;
				if (scopeAttachment->GetUsage() == RHI::ScopeAttachmentUsage::Shader ||
					scopeAttachment->GetUsage() == RHI::ScopeAttachmentUsage::Copy)
					continue;
				ImageScopeAttachment* imageScopeAttachment = (ImageScopeAttachment*)scopeAttachment;
				ImageFrameAttachment* imageFrameAttachment = (ImageFrameAttachment*)scopeAttachment->GetFrameAttachment();
				RHI::Format format = imageFrameAttachment->GetImageDescriptor().format;

				AttachmentBinding attachmentBinding{};
				attachmentBinding.format = format;
				attachmentBinding.loadStoreAction = scopeAttachment->GetLoadStoreAction();
				attachmentBinding.multisampleState = scopeAttachment->GetMultisampleState();

				SubPassAttachment attachmentRef{};

				switch (scopeAttachment->GetUsage())
				{
				case RHI::ScopeAttachmentUsage::DepthStencil:
					attachmentRef.attachmentIndex = i++;
					if (EnumHasFlag(scopeAttachment->GetAccess(), RHI::ScopeAttachmentAccess::Write))
					{
						attachmentBinding.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
						attachmentBinding.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
						attachmentRef.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					}
					else
					{
						attachmentBinding.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
						attachmentBinding.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
						attachmentRef.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
					}
					subpass.depthStencilAttachment.Add(attachmentRef);
					break;
				case RHI::ScopeAttachmentUsage::RenderTarget:
					attachmentRef.attachmentIndex = i++;
					attachmentRef.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
					attachmentBinding.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					attachmentBinding.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					break;
				case RHI::ScopeAttachmentUsage::SubpassInput:
					attachmentBinding.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					attachmentBinding.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					break;
				case RHI::ScopeAttachmentUsage::Resolve:
					attachmentBinding.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					attachmentBinding.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					break;
				}
				
				outDescriptor.attachments.Add(attachmentBinding);
				attachmentBindingsById[scopeAttachment->GetId()] = &outDescriptor.attachments.GetLast();
			}
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

	SIZE_T RenderPass::Descriptor::GetHash() const
	{
		SIZE_T hash = 0;
		for (const auto& attachment : attachments)
		{
			if (hash == 0)
				hash = attachment.GetHash();
			else
				CombineHash(hash, attachment);
		}
		for (const auto& subpass : subpasses)
		{
			if (hash == 0)
				hash = subpass.GetHash();
			else
				CombineHash(hash, subpass);
		}
		for (const auto& dependency : dependencies)
		{
			if (hash == 0)
				hash = CE::GetHash(dependency);
			else
				CombineHash(hash, dependency);
		}
		return hash;
	}

	SIZE_T RenderPass::SubPassDescriptor::GetHash() const
	{
		SIZE_T hash = 0;
		for (const auto& renderTargetAttachment : renderTargetAttachments)
		{
			if (hash == 0)
				hash = renderTargetAttachment.GetHash();
			else
				hash = GetCombinedHash(hash, renderTargetAttachment.GetHash());
		}
		for (const auto& resolveAttachment : resolveAttachments)
		{
			if (hash == 0)
				hash = resolveAttachment.GetHash();
			else
				hash = GetCombinedHash(hash, resolveAttachment.GetHash());
		}
		for (const auto& subpassInputAttachment : subpassInputAttachments)
		{
			if (hash == 0)
				hash = subpassInputAttachment.GetHash();
			else
				hash = GetCombinedHash(hash, subpassInputAttachment.GetHash());
		}
		for (auto preserveAttachment : preserveAttachments)
		{
			if (hash == 0)
				hash = CE::GetHash(preserveAttachment);
			else
				hash = GetCombinedHash(hash, CE::GetHash(preserveAttachment));
		}
		for (const auto& depthStencilAttachment : depthStencilAttachment)
		{
			if (hash == 0)
				hash = depthStencilAttachment.GetHash();
			else
				hash = GetCombinedHash(hash, depthStencilAttachment.GetHash());
		}

		return hash;
	}

	SIZE_T RenderPass::SubPassAttachment::GetHash() const
	{
		if (!IsValid())
			return 0;
		SIZE_T hash = (SIZE_T)attachmentIndex;
		hash = GetCombinedHash(hash, (SIZE_T)imageLayout);
		return hash;
	}

	SIZE_T RenderPass::AttachmentBinding::GetHash() const
	{
		SIZE_T hash = (SIZE_T)format;
		hash = GetCombinedHash(hash, CE::GetHash((int)loadStoreAction.loadAction));
		hash = GetCombinedHash(hash, CE::GetHash((int)loadStoreAction.storeAction));
		if (IsDepthStencilFormat(format))
		{
			hash = GetCombinedHash(hash, CE::GetHash((int)loadStoreAction.loadActionStencil));
			hash = GetCombinedHash(hash, CE::GetHash((int)loadStoreAction.storeActionStencil));
			hash = GetCombinedHash(hash, CE::GetHash<f32>(loadStoreAction.clearValueDepth));
			hash = GetCombinedHash(hash, CE::GetHash<f32>(loadStoreAction.clearValueStencil));
		}
		else if (IsDepthFormat(format))
		{
			hash = GetCombinedHash(hash, CE::GetHash<f32>(loadStoreAction.clearValueDepth));
		}
		else
		{
			hash = GetCombinedHash(hash, CE::GetHash(loadStoreAction.clearValue));
		}
		hash = GetCombinedHash(hash, (SIZE_T)initialLayout);
		hash = GetCombinedHash(hash, (SIZE_T)finalLayout);
		hash = GetCombinedHash(hash, (SIZE_T)multisampleState.sampleCount);
		return hash;
	}

} // namespace CE

