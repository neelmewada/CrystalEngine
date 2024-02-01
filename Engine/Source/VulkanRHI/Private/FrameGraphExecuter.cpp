#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

	FrameGraphExecuter::FrameGraphExecuter(VulkanDevice* device) : device(device)
	{
		
	}

	FrameGraphExecuter::~FrameGraphExecuter()
	{
		device->GetShaderResourceManager()->DestroyQueuedSRG();
	}

	bool FrameGraphExecuter::ExecuteInternal(const FrameGraphExecuteRequest& executeRequest)
	{
		device->GetShaderResourceManager()->DestroyQueuedSRG();

		FrameGraph* frameGraph = executeRequest.frameGraph;
		compiler = (Vulkan::FrameGraphCompiler*)executeRequest.compiler;
		Vulkan::Scope* presentingScope = (Vulkan::Scope*)frameGraph->presentingScope;
		auto swapChain = (Vulkan::SwapChain*)frameGraph->presentSwapChain;

		const Array<RHI::Scope*>& producers = frameGraph->producers;
		constexpr u64 u64Max = NumericLimits<u64>::Max();
		VkResult result = VK_SUCCESS;

		if (swapChain && presentingScope)
		{
            vkResetFences(device->GetHandle(), 1, &compiler->imageAcquiredFences[currentSubmissionIndex]);
            
			result = vkAcquireNextImageKHR(device->GetHandle(), swapChain->GetHandle(), u64Max,
				compiler->imageAcquiredSemaphores[currentSubmissionIndex],
				compiler->imageAcquiredFences[currentSubmissionIndex],
				&swapChain->currentImageIndex);
            
            vkWaitForFences(device->GetHandle(), 1, &compiler->imageAcquiredFences[currentSubmissionIndex], VK_TRUE, u64Max);

            // TODO: graphExecutionFences not working!
			vkWaitForFences(device->GetHandle(),
				compiler->graphExecutionFences[currentImageIndex].GetSize(),
				compiler->graphExecutionFences[currentImageIndex].GetData(),
				VK_TRUE, u64Max);
            
			
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

	void FrameGraphExecuter::WaitUntilIdle()
	{
		// Improve this code later
		vkDeviceWaitIdle(device->GetHandle());

		/*if (compiler)
		{
			constexpr u64 u64Max = NumericLimits<u64>::Max();

			List<VkFence> waitFences{};

			for (int i = 0; i < compiler->graphExecutionFences.GetSize(); i++)
			{
				int count = compiler->graphExecutionFences[i].GetSize();
				if (count <= 0)
					continue;
				for (int j = 0; j < count; j++)
				{
					if (compiler->graphExecutionFences[i][j] == nullptr)
						continue;

					waitFences.Add(compiler->graphExecutionFences[i][j]);
				}
			}

			vkWaitForFences(device->GetHandle(), waitFences.GetSize(), waitFences.GetData(), VK_TRUE, u64Max);
		}*/
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
		CommandList* commandList = compiler->commandListsByFamilyIndexPerImage[currentImageIndex][familyIndex];

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

		auto cmdBuffer = commandList->commandBuffer;

		commandList->Begin();
		{
			for (Vulkan::Scope* currentScope : scopeChain)
			{
				commandList->currentPass = currentScope->renderPass;
				commandList->currentSubpass = currentScope->subpassIndex;

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

					clearValues.Add(clearValue);
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

						for (const auto& transition : barrier.imageLayoutTransitions)
						{
							transition.image->curImageLayout = transition.layout;
							transition.image->curFamilyIndex = transition.queueFamilyIndex;
						}
					}
				}

				// Do image layout transitions manually
				for (auto scopeAttachment : currentScope->attachments)
				{
					if (!scopeAttachment->IsImageAttachment() || scopeAttachment->GetFrameAttachment() == nullptr ||
						!scopeAttachment->GetFrameAttachment()->IsImageAttachment())
						continue;

					ImageScopeAttachment* imageScopeAttachment = (ImageScopeAttachment*)scopeAttachment;
					ImageFrameAttachment* imageFrameAttachment = (ImageFrameAttachment*)scopeAttachment->GetFrameAttachment();

					RHIResource* resource = imageFrameAttachment->GetResource(currentImageIndex);
					if (resource == nullptr || resource->GetResourceType() != ResourceType::Texture)
						continue;

					Vulkan::Texture* image = (Vulkan::Texture*)resource;

					VkImageLayout requiredLayout = image->curImageLayout;
					VkPipelineStageFlags dstStageMask = 0;
					VkAccessFlags dstAccessMask = 0;
						
					switch (scopeAttachment->GetUsage())
					{
					case RHI::ScopeAttachmentUsage::RenderTarget:
					case RHI::ScopeAttachmentUsage::Resolve:
						requiredLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
						dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
						dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
						break;
					case RHI::ScopeAttachmentUsage::DepthStencil:
						dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
						if (EnumHasFlag(scopeAttachment->GetAccess(), RHI::ScopeAttachmentAccess::Write))
						{
							requiredLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
							dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
						}
						else // Read only
						{
							requiredLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
							dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
						}
						break;
					case RHI::ScopeAttachmentUsage::SubpassInput:
					case RHI::ScopeAttachmentUsage::Shader:
						dstStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
						if (EnumHasFlag(scopeAttachment->GetAccess(), RHI::ScopeAttachmentAccess::Write))
						{
							requiredLayout = VK_IMAGE_LAYOUT_GENERAL;
							dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
						}
						else
						{
							requiredLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
							dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
						}
						break;
					case RHI::ScopeAttachmentUsage::Copy:
						dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
						if (EnumHasFlag(scopeAttachment->GetAccess(), RHI::ScopeAttachmentAccess::Write))
						{
							requiredLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
							dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
						}
						else
						{
							requiredLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
							dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
						}
						break;
					default:
						continue;
					}

					if (image->curImageLayout != requiredLayout)
					{
						VkImageMemoryBarrier imageBarrier{};
						imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
						if (image->curFamilyIndex < 0)
							image->curFamilyIndex = presentQueue->GetFamilyIndex();
						imageBarrier.srcQueueFamilyIndex = image->curFamilyIndex;
						imageBarrier.dstQueueFamilyIndex = currentScope->queue->GetFamilyIndex();
						imageBarrier.image = image->GetImage();
						imageBarrier.oldLayout = image->curImageLayout;
						imageBarrier.newLayout = requiredLayout;
						imageBarrier.srcAccessMask = 0;
						imageBarrier.dstAccessMask = dstAccessMask;

						imageBarrier.subresourceRange.aspectMask = image->aspectMask;
						imageBarrier.subresourceRange.baseMipLevel = 0;
						imageBarrier.subresourceRange.levelCount = 1;
						imageBarrier.subresourceRange.baseArrayLayer = 0;
						imageBarrier.subresourceRange.layerCount = 1;

						vkCmdPipelineBarrier(cmdBuffer,
							VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
							dstStageMask,
							0,
							0, nullptr,
							0, nullptr,
							1, &imageBarrier);

						image->curImageLayout = requiredLayout;
						image->curFamilyIndex = currentScope->queue->GetFamilyIndex();
					}
				}

				// Graphics operation
				if (currentScope->queueClass == RHI::HardwareQueueClass::Graphics)
				{
					VkRenderPassBeginInfo beginInfo{};
					beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
					beginInfo.renderPass = renderPass->GetHandle();
					FrameBuffer* frameBuffer = currentScope->frameBuffers[currentImageIndex];
					beginInfo.framebuffer = frameBuffer->GetHandle();
					beginInfo.clearValueCount = clearValues.GetSize();
					beginInfo.pClearValues = clearValues.GetData();

					beginInfo.renderArea.offset.x = 0;
					beginInfo.renderArea.offset.y = 0;
					beginInfo.renderArea.extent.width = frameBuffer->GetWidth();
					beginInfo.renderArea.extent.height = frameBuffer->GetHeight();

					for (auto srg : currentScope->externalShaderResourceGroups)
					{
						commandList->SetShaderResourceGroup(srg);
					}

					if (currentScope->passShaderResourceGroup)
						commandList->SetShaderResourceGroup(currentScope->passShaderResourceGroup);
					if (currentScope->subpassShaderResourceGroup)
						commandList->SetShaderResourceGroup(currentScope->subpassShaderResourceGroup);

					vkCmdBeginRenderPass(cmdBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
					{
						VkViewport viewport{};
						viewport.x = viewport.y = 0;
						viewport.width = frameBuffer->GetWidth();
						viewport.height = frameBuffer->GetHeight();
						viewport.minDepth = 0.0f;
						viewport.maxDepth = 1.0f;
						vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

						VkRect2D scissor{};
						scissor.offset.x = scissor.offset.y = 0;
						scissor.extent.width = viewport.width;
						scissor.extent.height = viewport.height;
						vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
						
						RHI::DrawList* drawList = currentScope->drawList;

						for (int i = 0; drawList != nullptr && i < drawList->GetDrawItemCount(); i++)
						{
							const auto& drawItemProperties = drawList->GetDrawItem(i);
							const DrawItem* drawItem = drawItemProperties.item;
							
							if (drawItem->enabled)
							{
								// Bind Pipeline
								RHI::PipelineState* pipeline = drawItem->pipelineState;
								if (pipeline)
									commandList->BindPipelineState(pipeline);

								// Bind SRGs
								for (int j = 0; j < drawItem->shaderResourceGroupCount; j++)
								{
									if (drawItem->shaderResourceGroups[j] != nullptr)
									{
										commandList->SetShaderResourceGroup(drawItem->shaderResourceGroups[j]);
									}
								}

								if (drawItem->uniqueShaderResourceGroup != nullptr)
								{
									commandList->SetShaderResourceGroup(drawItem->uniqueShaderResourceGroup);
								}

								// Commit SRGs
								commandList->CommitShaderResources();

								// Draw Indexed
								commandList->BindVertexBuffers(0, drawItem->vertexBufferViewCount, drawItem->vertexBufferViews);
								commandList->BindIndexBuffer(*drawItem->indexBufferView);

								commandList->DrawIndexed(drawItem->arguments.indexedArgs);
							}
						}
					}
					vkCmdEndRenderPass(cmdBuffer);
				}
				else if (currentScope->queueClass == RHI::HardwareQueueClass::Compute)
				{
					// TODO: Add compute pass
					RHI::PipelineState* pipelineToUse = nullptr;

					for (RHI::PipelineState* pipeline : currentScope->usePipelines)
					{
						if (pipeline != nullptr && pipeline->GetPipelineType() == RHI::PipelineStateType::Compute)
						{
							pipelineToUse = pipeline;
						}
					}

					if (pipelineToUse != nullptr)
					{
						for (auto srg : currentScope->externalShaderResourceGroups)
						{
							commandList->SetShaderResourceGroup(srg);
						}

						if (currentScope->passShaderResourceGroup)
							commandList->SetShaderResourceGroup(currentScope->passShaderResourceGroup);
						if (currentScope->subpassShaderResourceGroup)
							commandList->SetShaderResourceGroup(currentScope->subpassShaderResourceGroup);

						commandList->Dispatch(Math::Max((u32)1, currentScope->groupCountX),
							Math::Max((u32)1, currentScope->groupCountY),
							Math::Max((u32)1, currentScope->groupCountZ));
					}					
				}
				else if (currentScope->queueClass == RHI::HardwareQueueClass::Transfer)
				{
					// TODO: Add transfer pass
				}
			}

			commandList->currentPass = nullptr;
			commandList->currentSubpass = 0;;
			
			// Transition to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			if (presentRequired)
			{
				Vulkan::Texture* image = (Vulkan::Texture*)swapChain->GetCurrentImage();
				if (image->curFamilyIndex < 0)
					image->curFamilyIndex = presentQueue->GetFamilyIndex();

				VkImageMemoryBarrier imageBarrier{};
				imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				if (image->curFamilyIndex < 0)
					image->curFamilyIndex = presentQueue->GetFamilyIndex();
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

				image->curImageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				image->curFamilyIndex = presentQueue->GetFamilyIndex();
			}
		}
		commandList->End();
        
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

		for (RHI::Scope* consumerScope : scopeChain.Top()->consumers)
		{
			ExecuteScope(executeRequest, (Vulkan::Scope*)consumerScope);
		}

		return result == VK_SUCCESS;
	}
    
} // namespace CE::Vulkan
