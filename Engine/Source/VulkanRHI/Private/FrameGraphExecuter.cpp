#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

	FrameGraphExecuter::FrameGraphExecuter(VulkanDevice* device) : device(device)
	{
		
	}

	FrameGraphExecuter::~FrameGraphExecuter()
	{

	}

	bool FrameGraphExecuter::ExecuteInternal(const FrameGraphExecuteRequest& executeRequest)
	{
		FrameGraph* frameGraph = executeRequest.frameGraph;
		FrameGraphCompiler* compiler = (Vulkan::FrameGraphCompiler*)executeRequest.compiler;
		Vulkan::Scope* presentingScope = (Vulkan::Scope*)frameGraph->presentingScope;
		auto swapChain = (Vulkan::SwapChain*)frameGraph->presentSwapChain;

		const Array<RHI::Scope*>& producers = frameGraph->producers;
		constexpr u64 u64Max = NumericLimits<u64>::Max();
		VkResult result = VK_SUCCESS;

		if (swapChain && presentingScope)
		{
			result = vkResetFences(device->GetHandle(), 1, &compiler->imageAcquiredFences[currentSubmissionIndex]);

			result = vkAcquireNextImageKHR(device->GetHandle(), swapChain->GetHandle(), u64Max, 
				compiler->imageAcquiredSemaphores[currentSubmissionIndex],
				compiler->imageAcquiredFences[currentSubmissionIndex], &swapChain->currentImageIndex);
			
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				swapChain->RebuildSwapChain();
				return false; // TODO: Try acquiring image again...
			}

			currentImageIndex = swapChain->currentImageIndex;
		}
		else
		{
			//currentImageIndex = currentSubmissionIndex;
		}

		bool success = true;
        
		for (auto rhiScope : frameGraph->producers)
		{
			success = ExecuteScope(executeRequest, (Vulkan::Scope*)rhiScope);
		}

		currentSubmissionIndex = (currentSubmissionIndex + 1) % compiler->imageCount;

		return success && result == VK_SUCCESS;
	}

	bool FrameGraphExecuter::ExecuteScope(const FrameGraphExecuteRequest& executeRequest, Vulkan::Scope* scope)
	{
		if (!scope)
			return false;



		FrameGraph* frameGraph = executeRequest.frameGraph;
		FrameGraphCompiler* compiler = (Vulkan::FrameGraphCompiler*)executeRequest.compiler;
		Vulkan::Scope* presentingScope = (Vulkan::Scope*)frameGraph->presentingScope;
		auto swapChain = (Vulkan::SwapChain*)frameGraph->presentSwapChain;
		bool presentRequired = false;
		auto presentQueue = device->GetPresentQueue();
		bool isSwapChainImageUsed = false;
		
		const RHI::FrameGraph::GraphNode& graphNode = frameGraph->nodes[scope->id];
		
		constexpr u64 u64Max = NumericLimits<u64>::Max();
		VkResult result = VK_SUCCESS;

		// Wait for rendering from earlier submission to finish.
		// We cannot record new commands into a command buffer that is currently being executed.
		result = vkWaitForFences(device->GetHandle(), 1, &scope->renderFinishedFences[currentImageIndex], VK_TRUE, u64Max);
		
		u32 familyIndex = scope->queue->GetFamilyIndex();
		CommandList* commandList = compiler->commandListsByImageIndex[currentImageIndex][familyIndex];

		Array<RHI::Scope*> consumers{};

		Array<Scope*> scopeChain{};
		Scope* scopeInChain = scope;
		while (scopeInChain != nullptr)
		{
			if (scopeInChain->usesSwapChainAttachment)
				isSwapChainImageUsed = true;
			scopeChain.Add(scopeInChain);
			scopeInChain = (Vulkan::Scope*)scopeInChain->next;
		}

		if (scope->consumers.GetSize() > 1)
		{
			consumers = scope->consumers;
		}
		else
		{
			consumers = scopeChain.Top()->consumers;
		}

		if (scopeChain.Top()->PresentsSwapChain())
			presentRequired = true;

		VkCommandBufferBeginInfo cmdBeginInfo{};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		//cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		auto cmdBuffer = commandList->commandBuffer;

		vkBeginCommandBuffer(cmdBuffer, &cmdBeginInfo);
		{
			for (Vulkan::Scope* currentScope : scopeChain)
			{
				bool usesSwapChainAttachment = currentScope->usesSwapChainAttachment;
				RenderPass* renderPass = currentScope->renderPass;
				FixedArray<VkClearValue, RHI::Limits::Pipeline::MaxRenderAttachmentCount> clearValues{};

				for (auto scopeAttachment : currentScope->attachments)
				{
					if (!scopeAttachment->IsImageAttachment() || scopeAttachment->GetFrameAttachment() == nullptr ||
						!scopeAttachment->GetFrameAttachment()->IsImageAttachment())
						continue;
					if (scopeAttachment->GetUsage() == RHI::ScopeAttachmentUsage::Shader ||
						scopeAttachment->GetUsage() == RHI::ScopeAttachmentUsage::Copy)
						continue;

					ImageScopeAttachment* imageScopeAttachment = (ImageScopeAttachment*)scopeAttachment;
					ImageFrameAttachment* imageFrameAttachment = (ImageFrameAttachment*)scopeAttachment->GetFrameAttachment();

					VkClearValue clearValue{};
					
					if (scopeAttachment->GetUsage() == RHI::ScopeAttachmentUsage::DepthStencil)
					{
						clearValue.depthStencil.depth = scopeAttachment->GetLoadStoreAction().clearValueDepth;
						clearValue.depthStencil.stencil = scopeAttachment->GetLoadStoreAction().clearValueStencil;
					}
					else
					{
						const RHI::AttachmentLoadStoreAction& loadStoreAction = scopeAttachment->GetLoadStoreAction();
						memcpy(clearValue.color.float32, loadStoreAction.clearValue.xyzw, sizeof(f32[4]));
					}

					if (imageFrameAttachment->IsSwapChainAttachment())
					{
						Vulkan::Texture* image = (Vulkan::Texture*)swapChain->GetCurrentImage();


					}

					clearValues.Add(clearValue);
				}

				if (usesSwapChainAttachment && swapChain->swapChainInitialImageLayouts[currentImageIndex] == VK_IMAGE_LAYOUT_UNDEFINED)
				{
					swapChain->swapChainInitialImageLayouts[currentImageIndex] = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

					VkImageMemoryBarrier imageBarrier{};
					imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
					imageBarrier.srcAccessMask = 0;
					imageBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imageBarrier.image = ((Vulkan::Texture*)swapChain->images[currentImageIndex])->GetImage();
					imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					imageBarrier.subresourceRange.baseArrayLayer = 0;
					imageBarrier.subresourceRange.layerCount = 1;
					imageBarrier.subresourceRange.baseMipLevel = 0;
					imageBarrier.subresourceRange.levelCount = 1;

					vkCmdPipelineBarrier(cmdBuffer,
						VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
						VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
						0,
						0, nullptr,
						0, nullptr,
						1, &imageBarrier);
				}

				if (currentScope->barriers[currentImageIndex].NonEmpty())
				{
					for (const auto& barrier : currentScope->barriers[currentImageIndex])
					{
						vkCmdPipelineBarrier(cmdBuffer,
							barrier.srcStageMask, barrier.dstStageMask,
							0,
							barrier.memoryBarriers.GetSize(), barrier.memoryBarriers.GetData(),
							barrier.bufferBarriers.GetSize(), barrier.bufferBarriers.GetData(),
							barrier.imageBarriers.GetSize(), barrier.imageBarriers.GetData());
					}
				}

				VkRenderPassBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				beginInfo.renderPass = renderPass->GetHandle();
				beginInfo.framebuffer = currentScope->frameBuffers[currentImageIndex]->GetHandle();
				beginInfo.clearValueCount = clearValues.GetSize();
				beginInfo.pClearValues = clearValues.GetData();

				beginInfo.renderArea.offset.x = 0;
				beginInfo.renderArea.offset.y = 0;
				beginInfo.renderArea.extent.width = currentScope->frameBuffers[currentImageIndex]->GetWidth();
				beginInfo.renderArea.extent.height = currentScope->frameBuffers[currentImageIndex]->GetHeight();

				vkCmdBeginRenderPass(cmdBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
				{
					// Execute scope
				}
				vkCmdEndRenderPass(cmdBuffer);
			}
			
			// Transition to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			if (presentRequired)
			{
				Vulkan::Texture* image = (Vulkan::Texture*)swapChain->GetCurrentImage();
				if (image->curFamilyIndex < 0)
					image->curFamilyIndex = presentQueue->GetFamilyIndex();

				VkImageMemoryBarrier imageBarrier{};
				imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageBarrier.srcQueueFamilyIndex = image->curFamilyIndex;
				imageBarrier.dstQueueFamilyIndex = presentQueue->GetFamilyIndex();
				imageBarrier.image = image->image;
				imageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				imageBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				imageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				imageBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

				imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageBarrier.subresourceRange.baseMipLevel = 0;
				imageBarrier.subresourceRange.levelCount = 1;
				imageBarrier.subresourceRange.baseArrayLayer = 0;
				imageBarrier.subresourceRange.layerCount = 1;
				
				vkCmdPipelineBarrier(cmdBuffer,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
					0,
					0, nullptr,
					0, nullptr,
					1, &imageBarrier);
			}
		}
		vkEndCommandBuffer(cmdBuffer);
        
        // Manually reset (close) the fence. i.e. put it in unsignalled state
        result = vkResetFences(device->GetHandle(), 1, &scope->renderFinishedFences[currentImageIndex]);

		static List<VkSemaphore> waitSemaphores{};
		static List<VkPipelineStageFlags> waitStages{};

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		
		if (isSwapChainImageUsed) // Frame graph uses a swapchain image
		{
			submitInfo.waitSemaphoreCount = scope->waitSemaphores[currentImageIndex].GetSize() + 1;
			if (waitSemaphores.GetSize() < submitInfo.waitSemaphoreCount)
				waitSemaphores.Resize(submitInfo.waitSemaphoreCount);
			if (waitStages.GetSize() < submitInfo.waitSemaphoreCount)
				waitStages.Resize(submitInfo.waitSemaphoreCount);

			for (int i = 0; i < scope->waitSemaphores[currentImageIndex].GetSize(); i++)
			{
				waitSemaphores[i] = scope->waitSemaphores[currentImageIndex][i];
				waitStages[i] = scope->waitSemaphoreStageFlags[i];
			}

			waitSemaphores[submitInfo.waitSemaphoreCount - 1] = compiler->imageAcquiredSemaphores[currentSubmissionIndex];
			waitStages[submitInfo.waitSemaphoreCount - 1] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			submitInfo.pWaitSemaphores = waitSemaphores.GetData();
			submitInfo.pWaitDstStageMask = waitStages.GetData();
			submitInfo.signalSemaphoreCount = 1;
		}
		else
		{
			submitInfo.waitSemaphoreCount = scope->waitSemaphores[currentImageIndex].GetSize();
			if (submitInfo.waitSemaphoreCount > 0)
			{
				submitInfo.pWaitSemaphores = scope->waitSemaphores[currentImageIndex].GetData();
				submitInfo.pWaitDstStageMask = scope->waitSemaphoreStageFlags.GetData();
			}
			submitInfo.signalSemaphoreCount = 1;
		}
		submitInfo.pSignalSemaphores = &scope->renderFinishedSemaphores[currentImageIndex];
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandList->commandBuffer;
		
		result = vkQueueSubmit(scope->queue->GetHandle(), 1, &submitInfo, scope->renderFinishedFences[currentImageIndex]);

		if (presentRequired && swapChain != nullptr)
		{
			VkSwapchainKHR swapchainKhr = swapChain->GetHandle();

			VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &swapchainKhr;
			presentInfo.pImageIndices = &currentImageIndex;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = &scope->renderFinishedSemaphores[currentImageIndex];

			result = vkQueuePresentKHR(presentQueue->GetHandle(), &presentInfo);
		}

		return result == VK_SUCCESS;
	}
    
} // namespace CE::Vulkan
