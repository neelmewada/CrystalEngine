#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    
	GraphicsCommandList::GraphicsCommandList(VulkanRHI* vulkanRHI, VulkanDevice* device, VulkanViewport* viewport)
		: vulkanRHI(vulkanRHI)
		, device(device)
		, viewport(viewport)
	{
		this->renderTarget = (VulkanRenderTarget*)viewport->GetRenderTarget();
		this->numCommandBuffers = viewport->GetBackBufferCount();
		this->simultaneousFrameDraws = viewport->GetSimultaneousFrameDrawCount();

		VkCommandBufferAllocateInfo commandAllocInfo = {};
		commandAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandAllocInfo.commandBufferCount = numCommandBuffers;
		commandAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandAllocInfo.commandPool = device->GetGraphicsCommandPool();

		commandBuffers.Resize(numCommandBuffers);

		if (vkAllocateCommandBuffers(device->GetHandle(), &commandAllocInfo, commandBuffers.GetData()) != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to allocate Vulkan Command Buffers for a Graphics Command List object!");
			return;
		}

		CreateSyncObjects();
	}

	GraphicsCommandList::GraphicsCommandList(VulkanRHI* vulkanRHI, VulkanDevice* device, VulkanRenderTarget* renderTarget)
		: vulkanRHI(vulkanRHI)
		, device(device)
		, renderTarget(renderTarget)
	{
		if (renderTarget->IsViewportRenderTarget())
		{
			auto viewport = renderTarget->GetVulkanViewport();

			this->renderTarget = (VulkanRenderTarget*)viewport->GetRenderTarget();
			this->numCommandBuffers = viewport->GetBackBufferCount();
			this->simultaneousFrameDraws = viewport->GetSimultaneousFrameDrawCount();

			VkCommandBufferAllocateInfo commandAllocInfo = {};
			commandAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandAllocInfo.commandBufferCount = numCommandBuffers;
			commandAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandAllocInfo.commandPool = device->GetGraphicsCommandPool();

			commandBuffers.Resize(numCommandBuffers);

			if (vkAllocateCommandBuffers(device->GetHandle(), &commandAllocInfo, commandBuffers.GetData()) != VK_SUCCESS)
			{
				CE_LOG(Error, All, "Failed to allocate Vulkan Command Buffers for a Graphics Command List object!");
				return;
			}

			CreateSyncObjects();
			return;
		}
		else
		{
			this->numCommandBuffers = renderTarget->GetBackBufferCount();
			this->simultaneousFrameDraws = renderTarget->GetBackBufferCount();

			VkCommandBufferAllocateInfo commandAllocInfo = {};
			commandAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandAllocInfo.commandBufferCount = numCommandBuffers;
			commandAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandAllocInfo.commandPool = device->GetGraphicsCommandPool();

			commandBuffers.Resize(numCommandBuffers);

			if (vkAllocateCommandBuffers(device->GetHandle(), &commandAllocInfo, commandBuffers.GetData()) != VK_SUCCESS)
			{
				CE_LOG(Error, All, "Failed to allocate Vulkan Command Buffers for a Graphics Command List object!");
				return;
			}

			CreateSyncObjects();
		}
	}

	GraphicsCommandList::~GraphicsCommandList()
	{
		vkDeviceWaitIdle(device->GetHandle());

		vkFreeCommandBuffers(device->GetHandle(), device->GetGraphicsCommandPool(), numCommandBuffers, commandBuffers.GetData());

		DestroySyncObjects();
	}

	void GraphicsCommandList::Begin()
	{
		constexpr auto u64Max = std::numeric_limits<u64>::max();

		vkWaitForFences(device->GetHandle(), renderFinishedFence.GetSize(), renderFinishedFence.GetData(), VK_TRUE, u64Max);

		VkExtent2D extent{};
		extent.width = renderTarget->GetWidth();
		extent.height = renderTarget->GetHeight();

		// - Command Buffer & Render Pass --
		VkCommandBufferBeginInfo cmdBufferInfo{};
		cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = ((VulkanRenderPass*)renderTarget->GetRenderPass())->GetHandle();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = extent;

		VkClearValue clearValues[RHI::MaxSimultaneousRenderOutputs + 1] = {};
		for (int i = 0; i < renderTarget->GetColorAttachmentCount(); ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				clearValues[i].color.float32[j] = renderTarget->clearColors[i][j];
			}
		}

		if (renderTarget->HasDepthStencilAttachment())
		{
			clearValues[renderTarget->GetColorAttachmentCount()].depthStencil.depth = 1.0f;
			clearValues[renderTarget->GetColorAttachmentCount()].depthStencil.stencil = 0;
		}

		renderPassInfo.clearValueCount = renderTarget->GetTotalAttachmentCount();
		renderPassInfo.pClearValues = clearValues;

		for (int i = 0; i < commandBuffers.GetSize(); i++)
		{
			if (viewport != nullptr)
			{
				renderPassInfo.framebuffer = viewport->frameBuffers[i]->GetHandle();
			}
			else
			{
				renderPassInfo.framebuffer = renderTarget->colorFrames[i].framebuffer->GetHandle();
			}

			// Begin Command Buffer
			vkBeginCommandBuffer(commandBuffers[i], &cmdBufferInfo);

			// Begin Render Pass
			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		}
	}

	void GraphicsCommandList::End()
	{
		for (int i = 0; i < commandBuffers.GetSize(); ++i)
		{
			vkCmdEndRenderPass(commandBuffers[i]);
			vkEndCommandBuffer(commandBuffers[i]);
		}
	}

	void GraphicsCommandList::SetScissorRects(u32 scissorCount, const RHI::ScissorState* scissors)
	{
		static Array<VkRect2D> scissorRects{};
		if (scissorRects.GetSize() < scissorCount)
		{
			scissorRects.Resize(scissorCount);
		}

		for (int i = 0; i < scissorCount; i++)
		{
			scissorRects[i].offset.x = scissors[i].x;
			scissorRects[i].offset.y = scissors[i].y;
			scissorRects[i].extent.width = scissors[i].width;
			scissorRects[i].extent.height = scissors[i].height;
		}

		for (int i = 0; i < commandBuffers.GetSize(); i++)
		{
			vkCmdSetScissor(commandBuffers[i], 0, scissorCount, scissorRects.GetData());
		}
	}

	void GraphicsCommandList::SetViewportRects(u32 viewportCount, const RHI::ViewportState* viewports)
	{
		static Array<VkViewport> viewportArray{};
		if (viewportArray.GetSize() < viewportCount)
		{
			viewportArray.Resize(viewportCount);
		}

		for (int i = 0; i < viewportCount; i++)
		{
			viewportArray[i].x = viewports[i].x;
			viewportArray[i].y = viewports[i].y;
			viewportArray[i].width = viewports[i].width;
			viewportArray[i].height = viewports[i].height;
			viewportArray[i].minDepth = viewports[i].minDepth;
			viewportArray[i].maxDepth = viewports[i].maxDepth;
		}

		for (int i = 0; i < commandBuffers.GetSize(); i++)
		{
			vkCmdSetViewport(commandBuffers[i], 0, viewportCount, viewportArray.GetData());
		}
	}

	void GraphicsCommandList::BindVertexBuffers(u32 firstBinding, const Array<RHI::Buffer*>& buffers)
	{
		List<VkBuffer> vkBuffers = {};
		Array<SIZE_T> offsets = {};
		vkBuffers.Resize(buffers.GetSize());
		offsets.Resize(buffers.GetSize());
		for (int i = 0; i < buffers.GetSize(); i++)
		{
			vkBuffers[i] = (VkBuffer)buffers[i]->GetHandle();
			offsets[i] = 0;
		}

		for (int i = 0; i < commandBuffers.GetSize(); ++i)
		{
			vkCmdBindVertexBuffers(commandBuffers[i], firstBinding, vkBuffers.GetSize(), vkBuffers.GetData(), offsets.GetData());
		}
	}

	void GraphicsCommandList::BindVertexBuffers(u32 firstBinding, const Array<RHI::Buffer*>& buffers, const Array<SIZE_T>& bufferOffsets)
	{
		if (buffers.GetSize() != bufferOffsets.GetSize())
		{
			CE_LOG(Error, All, "BindVertexBuffers() passed with buffers & bufferOffsets array of different size!");
			return;
		}

		List<VkBuffer> vkBuffers = {};
		vkBuffers.Resize(buffers.GetSize());
		for (int i = 0; i < buffers.GetSize(); i++)
		{
			vkBuffers[i] = (VkBuffer)buffers[i]->GetHandle();
		}

		for (int i = 0; i < commandBuffers.GetSize(); ++i)
		{
			vkCmdBindVertexBuffers(commandBuffers[i], firstBinding, vkBuffers.GetSize(), vkBuffers.GetData(), bufferOffsets.GetData());
		}
	}

	void GraphicsCommandList::BindIndexBuffer(RHI::Buffer* buffer, bool use32BitIndex, SIZE_T offset)
	{
		for (int i = 0; i < commandBuffers.GetSize(); ++i)
		{
			vkCmdBindIndexBuffer(commandBuffers[i], (VkBuffer)buffer->GetHandle(), offset, use32BitIndex ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16);
		}
	}

	void GraphicsCommandList::BindPipeline(RHI::IPipelineState* pipeline)
	{
		if (pipeline == nullptr)
			return;

		VkPipeline vkPipeline = (VkPipeline)pipeline->GetNativeHandle();

		RHI::IPipelineLayout* pipelineLayout = pipeline->GetPipelineLayout();

		VkPipelineBindPoint bindPoint{};

		switch (pipelineLayout->GetPipelineType())
		{
		case RHI::PipelineType::None:
			return;
		case RHI::PipelineType::Graphics:
			bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			break;
		case RHI::PipelineType::Compute:
			bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
			break;
		}

		for (int i = 0; i < commandBuffers.GetSize(); ++i)
		{
			vkCmdBindPipeline(commandBuffers[i], bindPoint, vkPipeline);
		}

		currentPipelineLayout = (VulkanPipelineLayout*)pipeline->GetPipelineLayout();
	}

	void GraphicsCommandList::CommitShaderResources(const Array<RHI::ShaderResourceGroup*>& shaderResourceGroups)
	{
		if (shaderResourceGroups.IsEmpty())
			return;

		if (srgManager == nullptr)
			srgManager = device->GetShaderResourceManager();

		Array<ShaderResourceGroup*> srgs = shaderResourceGroups.Transform<ShaderResourceGroup*>(
			[&](RHI::ShaderResourceGroup* in) { return (ShaderResourceGroup*)in; });

		for (ShaderResourceGroup* srg : srgs)
		{
			
		}
		
	}

	void GraphicsCommandList::SetRootConstants(u8 size, const u8* data)
	{
		for (int i = 0; i < commandBuffers.GetSize(); ++i)
		{
			vkCmdPushConstants(commandBuffers[i], currentPipelineLayout->GetNativeHandle(), VK_SHADER_STAGE_ALL, 0, 12, data);
		}
	}

	void GraphicsCommandList::DrawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex, s32 vertexOffset, u32 firstInstance)
	{
		for (int i = 0; i < commandBuffers.GetSize(); ++i)
		{
			/*for (auto [setNumber, srg] : boundMainSRGBySetNumber)
			{
				if (!modifiedDescriptorSetNumbers.Exists(setNumber))
					continue;

				modifiedDescriptorSetNumbers.Remove(setNumber);

				VkDescriptorSet set = srg->GetDescriptorSet();
				vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, currentPipelineLayout->GetNativeHandle(),
					setNumber, 1, &set, 0, nullptr);
			}*/
		}

		for (int i = 0; i < commandBuffers.GetSize(); ++i)
		{
			vkCmdDrawIndexed(commandBuffers[i], indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
		}
	}

	void GraphicsCommandList::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreCI{};
		semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		renderFinishedSemaphore.Resize(numCommandBuffers);
		for (int i = 0; i < numCommandBuffers; i++)
		{
			if (vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &renderFinishedSemaphore[i]) != VK_SUCCESS)
			{
				CE_LOG(Error, All, "Failed to create render finished semaphore for a Vulkan Graphics Command List");
				return;
			}
		}

		VkFenceCreateInfo fenceCI = {};
		fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Create fence in the signaled state (i.e. fence is open)

		renderFinishedFence.Resize(numCommandBuffers);
		for (int i = 0; i < numCommandBuffers; i++)
		{
			if (vkCreateFence(device->GetHandle(), &fenceCI, nullptr, &renderFinishedFence[i]) != VK_SUCCESS)
			{
				CE_LOG(Error, All, "Failed to create render finished fence for a Vulkan Graphics Command List");
				return;
			}
		}
	}

	void GraphicsCommandList::DestroySyncObjects()
	{
		for (int i = 0; i < renderFinishedFence.GetSize(); ++i)
		{
			vkDestroyFence(device->GetHandle(), renderFinishedFence[i], nullptr);
		}
		renderFinishedFence.Clear();

		for (int i = 0; i < renderFinishedSemaphore.GetSize(); ++i)
		{
			vkDestroySemaphore(device->GetHandle(), renderFinishedSemaphore[i], nullptr);
		}
		renderFinishedSemaphore.Clear();
	}

} // namespace CE::Vulkan
