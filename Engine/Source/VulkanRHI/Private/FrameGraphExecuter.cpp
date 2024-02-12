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
				//compiler->imageAcquiredFences[currentSubmissionIndex],
				nullptr,
				&swapChain->currentImageIndex);
            
			vkWaitForFences(device->GetHandle(),
				compiler->graphExecutionFences[currentImageIndex].GetSize(),
				compiler->graphExecutionFences[currentImageIndex].GetData(),
				VK_TRUE, u64Max);
			
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				swapChain->RebuildSwapChain();
				return false; // TODO: Try acquiring image again next frame...
			}

			currentImageIndex = swapChain->currentImageIndex;
		}
		else
		{
			currentImageIndex = currentSubmissionIndex;
		}

		bool success = true;

		HashSet<ScopeID> executedScopes{};

		for (auto rhiScope : frameGraph->endScopes)
		{
			ExecuteScope(executeRequest, (Vulkan::Scope*)rhiScope, executedScopes);
		}

		currentSubmissionIndex = (currentSubmissionIndex + 1) % compiler->imageCount;
		totalFramesSubmitted++;

		return success && result == VK_SUCCESS;
	}

	void FrameGraphExecuter::WaitUntilIdle()
	{
		// TODO: Improve this code later using fences
		vkDeviceWaitIdle(device->GetHandle());
	}

	u32 FrameGraphExecuter::BeginExecution(const FrameGraphExecuteRequest& executeRequest)
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
				//compiler->imageAcquiredFences[currentSubmissionIndex],
				nullptr,
				&swapChain->currentImageIndex);

			vkWaitForFences(device->GetHandle(),
				compiler->graphExecutionFences[currentImageIndex].GetSize(),
				compiler->graphExecutionFences[currentImageIndex].GetData(),
				VK_TRUE, u64Max);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				swapChain->RebuildSwapChain();
				return false; // TODO: Try acquiring image again next frame...
			}

			currentImageIndex = swapChain->currentImageIndex;
		}
		else
		{
			currentImageIndex = currentSubmissionIndex;
		}

		return currentImageIndex;
	}

	void FrameGraphExecuter::EndExecution(const FrameGraphExecuteRequest& executeRequest)
	{
		FrameGraph* frameGraph = executeRequest.frameGraph;

		HashSet<ScopeID> executedScopes{};

		for (auto rhiScope : frameGraph->endScopes)
		{
			ExecuteScope(executeRequest, (Vulkan::Scope*)rhiScope, executedScopes);
		}

		currentSubmissionIndex = (currentSubmissionIndex + 1) % compiler->imageCount;
		totalFramesSubmitted++;
	}

	void FrameGraphExecuter::ExecuteScopesRecursively(Vulkan::Scope* scope)
	{	

	}

	bool FrameGraphExecuter::ExecuteScope(const FrameGraphExecuteRequest& executeRequest, Vulkan::Scope* scope, HashSet<ScopeID>& executedScopes)
	{
		if (!scope)
			return false;

		for (auto rhiProducer : scope->producers)
		{
			ExecuteScope(executeRequest, (Vulkan::Scope*)rhiProducer, executedScopes);
		}

		if (executedScopes.Exists(scope->id))
			return false;
		if (scope->IsSubPass() && scope->prevSubPass != nullptr)
			return false;

		FrameGraph* frameGraph = executeRequest.frameGraph;
		FrameScheduler* scheduler = executeRequest.scheduler;
		FrameGraphCompiler* compiler = (Vulkan::FrameGraphCompiler*)executeRequest.compiler;
		Vulkan::Scope* presentingScope = (Vulkan::Scope*)frameGraph->presentingScope;
		auto swapChain = (Vulkan::SwapChain*)frameGraph->presentSwapChain;
		bool presentRequired = false;
		auto presentQueue = device->GetPresentQueue();
		bool isFirstSwapChainUse = false;
		
		const RHI::FrameGraph::GraphNode& graphNode = frameGraph->nodes[scope->id];
		
		constexpr u64 u64Max = NumericLimits<u64>::Max();
		VkResult result = VK_SUCCESS;

		// Wait for rendering from earlier submission to finish.
		// We cannot record new commands into a command buffer that is currently being executed.
		result = vkWaitForFences(device->GetHandle(), 1, &scope->renderFinishedFences[currentImageIndex], VK_TRUE, u64Max);
		
		u32 familyIndex = scope->queue->GetFamilyIndex();
		CommandList* commandList = scope->commandListsByFamilyIndexPerImage[currentImageIndex][familyIndex];

		Array<RHI::Scope*> consumers{};

		Array<Scope*> scopeChain{};
		Scope* scopeInChain = scope;

		while (scopeInChain != nullptr)
		{
			if (scopeInChain->usesSwapChainAttachment)
				isFirstSwapChainUse = true;
			scopeChain.Add(scopeInChain);
			scopeInChain = (Vulkan::Scope*)scopeInChain->next;
		}

		for (auto executedScopeId : executedScopes)
		{
			Vulkan::Scope* executedScope = (Vulkan::Scope*)frameGraph->scopesById[executedScopeId];
			if (executedScope->usesSwapChainAttachment)
			{
				isFirstSwapChainUse = false;
				break;
			}
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
			commandList->SetCurrentImageIndex(currentImageIndex);

			for (int scopeIndex = 0; scopeIndex < scopeChain.GetSize(); scopeIndex++)
			{
				Vulkan::Scope* currentScope = scopeChain[scopeIndex];
				if (currentScope == nullptr)
					continue;

				executedScopes.Add(currentScope->id);

				commandList->currentPass = currentScope->renderPass;
				commandList->currentSubpass = currentScope->subpassIndex;

				bool usesSwapChainAttachment = currentScope->usesSwapChainAttachment;
				RenderPass* renderPass = currentScope->renderPass;
				FixedArray<VkClearValue, RHI::Limits::Pipeline::MaxRenderAttachmentCount> clearValues{};
				HashSet<AttachmentID> clearedAttachments{};

				Vulkan::Scope* scopeLoop = currentScope;
				while (scopeLoop != nullptr)
				{
					for (auto scopeAttachment : scopeLoop->attachments)
					{
						if (!scopeAttachment->IsImageAttachment() || scopeAttachment->GetFrameAttachment() == nullptr ||
							!scopeAttachment->GetFrameAttachment()->IsImageAttachment())
							continue;
						if (scopeAttachment->GetUsage() == RHI::ScopeAttachmentUsage::Shader ||
							scopeAttachment->GetUsage() == RHI::ScopeAttachmentUsage::Copy)
							continue;
						if (clearedAttachments.Exists(scopeAttachment->GetFrameAttachment()->GetId()))
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

						clearedAttachments.Add(scopeAttachment->GetFrameAttachment()->GetId());

						clearValues.Add(clearValue);
					}

					scopeLoop = (Vulkan::Scope*)scopeLoop->nextSubPass;
				}

				// Execute compiled pipeline barriers (initial barriers)
				if (currentScope->initialBarriers[currentImageIndex].NonEmpty())
				{
					for (const auto& barrier : currentScope->initialBarriers[currentImageIndex])
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

						for (const auto& bufferTransition : barrier.bufferFamilyTransitions)
						{
							bufferTransition.buffer->curFamilyIndex = bufferTransition.queueFamilyIndex;
						}
					}
				}

				// Additional pipeline barriers if required, for VkImageLayout and/or queue family ownership transition
				// This is 'usually' only required for first-time use of an attachment
				// The FrameGraph handles the internal resource transitions through compiled pipeline barriers.
				{
					Vulkan::Scope* currentSubPassScope = currentScope;
					HashSet<AttachmentID> initializedAttachmentIds{};

					while (currentSubPassScope != nullptr)
					{
						// Do image-layout/buffer transitions manually (if required)
						
						for (auto scopeAttachment : currentSubPassScope->attachments)
						{
							if (!scopeAttachment->IsImageAttachment() || scopeAttachment->GetFrameAttachment() == nullptr ||
								!scopeAttachment->GetFrameAttachment()->IsImageAttachment())
								continue;

							ImageScopeAttachment* imageScopeAttachment = (ImageScopeAttachment*)scopeAttachment;
							ImageFrameAttachment* imageFrameAttachment = (ImageFrameAttachment*)scopeAttachment->GetFrameAttachment();

							RHIResource* resource = imageFrameAttachment->GetResource(currentImageIndex);
							if (resource == nullptr || resource->GetResourceType() != ResourceType::Texture)
								continue;
							if (initializedAttachmentIds.Exists(imageFrameAttachment->GetId()))
								continue;

							initializedAttachmentIds.Add(imageFrameAttachment->GetId());

							if (resource->GetResourceType() == ResourceType::Texture)
							{
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
									dstStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
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

								// Usually only required for Output image, because it transitions to PRESENT_SRC_KHR at the end of rendering.
								// Also useful when queue family index of image doesn't match to what's required.
								if (image->curImageLayout != requiredLayout || (image->curFamilyIndex >= 0 && image->curFamilyIndex != currentScope->queue->GetFamilyIndex()))
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
									imageBarrier.subresourceRange.levelCount = image->mipLevels;
									imageBarrier.subresourceRange.baseArrayLayer = 0;
									imageBarrier.subresourceRange.layerCount = image->arrayLayers;

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
							else if (resource->GetResourceType() == ResourceType::Buffer)
							{
								Vulkan::Buffer* buffer = (Vulkan::Buffer*)resource;

								if (buffer->curFamilyIndex >= 0 && buffer->curFamilyIndex != currentScope->queue->GetFamilyIndex())
								{
									VkPipelineStageFlags dstStageMask = 0;
									VkAccessFlags dstAccessMask = 0;

									switch (scopeAttachment->GetUsage())
									{
									case RHI::ScopeAttachmentUsage::Shader:
										dstStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
										if (EnumHasFlag(scopeAttachment->GetAccess(), RHI::ScopeAttachmentAccess::Write))
										{
											dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
										}
										else
										{
											dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
										}
										break;
									case RHI::ScopeAttachmentUsage::Copy:
										dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
										if (EnumHasFlag(scopeAttachment->GetAccess(), RHI::ScopeAttachmentAccess::Write))
										{
											dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
										}
										else
										{
											dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
										}
										break;
									default:
										continue;
									}

									VkBufferMemoryBarrier bufferBarrier{};
									bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
									bufferBarrier.srcAccessMask = 0;
									bufferBarrier.dstAccessMask = dstAccessMask;
									bufferBarrier.srcQueueFamilyIndex = buffer->curFamilyIndex;
									bufferBarrier.dstQueueFamilyIndex = currentScope->queue->GetFamilyIndex();
									bufferBarrier.buffer = buffer->GetBuffer();
									bufferBarrier.offset = 0;
									bufferBarrier.size = buffer->GetBufferSize();

									vkCmdPipelineBarrier(cmdBuffer,
										VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
										dstStageMask,
										0,
										0, nullptr,
										1, &bufferBarrier,
										0, nullptr);

									buffer->curFamilyIndex = currentScope->queue->GetFamilyIndex();
								}
							}
						}

						currentSubPassScope = (Vulkan::Scope*)currentSubPassScope->nextSubPass;
					}
				}

				bool shouldNotExecuteAtAll = false;
				bool shouldNotExecuteButShouldClear = false;

				for (const auto& cond : currentScope->executeConditions)
				{
					if (!frameGraph->VariableExists(cond.variableName))
					{
						shouldNotExecuteAtAll = true;
						break;
					}
					const auto& value = frameGraph->GetVariable(currentImageIndex, cond.variableName);
					bool result = cond.Compare(value);

					if (!result)
					{
						if (cond.shouldClear)
						{
							shouldNotExecuteButShouldClear = true;
							shouldNotExecuteAtAll = false;
						}
						else
						{
							shouldNotExecuteButShouldClear = false;
							shouldNotExecuteAtAll = true;
						}
						break;
					}
				}

				// Graphics operation
				if (!shouldNotExecuteAtAll && currentScope->queueClass == RHI::HardwareQueueClass::Graphics)
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

						while (!shouldNotExecuteButShouldClear && currentScope != nullptr)
						{
							RHI::DrawList* drawList = currentScope->drawList;

							// Submit draw items
							for (int i = 0; drawList != nullptr && i < drawList->GetDrawItemCount(); i++)
							{
								for (auto srg : currentScope->externalShaderResourceGroups)
								{
									commandList->SetShaderResourceGroup(srg);
								}

								if (currentScope->passShaderResourceGroup)
									commandList->SetShaderResourceGroup(currentScope->passShaderResourceGroup);
								if (currentScope->subpassShaderResourceGroup)
									commandList->SetShaderResourceGroup(currentScope->subpassShaderResourceGroup);

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

									for (int j = 0; j < drawItem->uniqueShaderResourceGroupCount; j++)
									{
										if (drawItem->uniqueShaderResourceGroups[j] != nullptr)
										{
											commandList->SetShaderResourceGroup(drawItem->uniqueShaderResourceGroups[j]);
										}
									}

									// Commit SRGs
									commandList->CommitShaderResources();

									// Draw Indexed
									commandList->BindVertexBuffers(0, drawItem->vertexBufferViewCount, drawItem->vertexBufferViews);
									commandList->BindIndexBuffer(*drawItem->indexBufferView);

									commandList->DrawIndexed(drawItem->arguments.indexedArgs);
								}
							}

							if (currentScope->nextSubPass == nullptr) // No more subpasses left
							{
								if (currentScope->IsSubPass())
								{
									const auto& renderPassDesc = currentScope->renderPass->desc;

									for (int i = 0; i < renderPassDesc.attachments.GetSize(); i++)
									{
										const auto& attachmentBinding = renderPassDesc.attachments[i];
										RHI::ScopeAttachment* scopeAttachment = currentScope->FindScopeAttachment(attachmentBinding.attachmentId);
										if (scopeAttachment == nullptr || scopeAttachment->GetFrameAttachment() == nullptr || 
											!scopeAttachment->GetFrameAttachment()->IsImageAttachment())
											continue;

										Vulkan::Texture* texture = (Vulkan::Texture*)scopeAttachment->GetFrameAttachment()->GetResource(currentImageIndex);
										if (texture == nullptr)
											continue;

										texture->curImageLayout = attachmentBinding.finalLayout;
									}
								}

								commandList->currentSubpass = 0;

								for (const auto& [variableName, value] : currentScope->setVariablesAfterExecution)
								{
									scheduler->SetFrameGraphVariable(currentImageIndex, variableName, value);
								}

								break;
							}
							else
							{
								scopeIndex++;
								currentScope = (Vulkan::Scope*)currentScope->nextSubPass;
								vkCmdNextSubpass(cmdBuffer, VK_SUBPASS_CONTENTS_INLINE);
								commandList->currentSubpass++;
								//commandList->ClearShaderResourceGroups();

								for (const auto& [variableName, value] : currentScope->setVariablesAfterExecution)
								{
									scheduler->SetFrameGraphVariable(currentImageIndex, variableName, value);
								}
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

				// Execute compiled pipeline barriers (exit barriers)
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

						for (const auto& bufferTransition : barrier.bufferFamilyTransitions)
						{
							bufferTransition.buffer->curFamilyIndex = bufferTransition.queueFamilyIndex;
						}
					}
				}
			}

			commandList->currentPass = nullptr;
			commandList->currentSubpass = 0;
			
			// Transition to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR if required
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
		
		if (isFirstSwapChainUse && scope->producers.IsEmpty()) // Frame graph uses a swapchain image
		{
			submitInfo.waitSemaphoreCount = scope->waitSemaphores[currentImageIndex].GetSize() + 1;
			if (waitSemaphores.GetSize() < submitInfo.waitSemaphoreCount)
				waitSemaphores.Resize(submitInfo.waitSemaphoreCount);
			if (waitStages.GetSize() < submitInfo.waitSemaphoreCount)
				waitStages.Resize(submitInfo.waitSemaphoreCount);
			
			// Wait semaphores from compiled FrameGraph, i.e. dependency on previous pass submissions.
			for (int i = 0; i < scope->waitSemaphores[currentImageIndex].GetSize(); i++)
			{
				waitSemaphores[i] = scope->waitSemaphores[currentImageIndex][i];
				waitStages[i] = scope->waitSemaphoreStageFlags[i];
			}

			// We need to wait on image acquired semaphore too
			waitSemaphores[submitInfo.waitSemaphoreCount - 1] = compiler->imageAcquiredSemaphores[currentSubmissionIndex];
            waitStages[submitInfo.waitSemaphoreCount - 1] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			submitInfo.pWaitSemaphores = waitSemaphores.GetData();
			submitInfo.pWaitDstStageMask = waitStages.GetData();
		}
		else
		{
			submitInfo.waitSemaphoreCount = scope->waitSemaphores[currentImageIndex].GetSize();
			if (submitInfo.waitSemaphoreCount > 0)
			{
				submitInfo.pWaitSemaphores = scope->waitSemaphores[currentImageIndex].GetData();
				submitInfo.pWaitDstStageMask = scope->waitSemaphoreStageFlags.GetData();
			}
		}
		submitInfo.signalSemaphoreCount = scope->signalSemaphores[currentImageIndex].GetSize();
		submitInfo.pSignalSemaphores = scope->signalSemaphores[currentImageIndex].GetData();//&scope->renderFinishedSemaphores[currentImageIndex];
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
			presentInfo.pWaitSemaphores = &scope->signalSemaphores[currentImageIndex][0];
            
			result = vkQueuePresentKHR(presentQueue->GetHandle(), &presentInfo);
		}

		return result == VK_SUCCESS;
	}
    
} // namespace CE::Vulkan
