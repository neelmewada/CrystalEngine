#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

    class QueueAllocator
    {
    public:
        
        void Init()
        {
            for (int familyIdx = 0; familyIdx < queuesByFamily.GetSize(); familyIdx++)
            {
				for (CommandQueue* queue : queuesByFamily[familyIdx])
				{
					auto mask = queue->GetQueueMask();
					if (EnumHasFlag(mask, RHI::HardwareQueueClassMask::Graphics))
					{
						graphicsQueues.Add(queue);
					}
					else if (EnumHasFlag(mask, RHI::HardwareQueueClassMask::Compute))
					{
						computeQueues.Add(queue);
					}
					else if (EnumHasFlag(mask, RHI::HardwareQueueClassMask::Transfer))
					{
						transferQueues.Add(queue);
					}
				}
            }
        }
        
        CommandQueue* Acquire(int trackNumber, RHI::HardwareQueueClass queueClass, bool present = false)
        {
			if (trackNumber < graphicsQueues.GetSize())
				return graphicsQueues[trackNumber];
			else if (queueClass == RHI::HardwareQueueClass::Compute && (trackNumber - graphicsQueues.GetSize()) < computeQueues.GetSize())
				return computeQueues[trackNumber - graphicsQueues.GetSize()];
            return graphicsQueues.GetLast();
        }
        
		Array<CommandQueue*> graphicsQueues{};
		Array<CommandQueue*> computeQueues{};
		Array<CommandQueue*> transferQueues{};
        
        CommandQueue* primaryQueue = nullptr;
        CommandQueue* presentQueue = nullptr;
        Array<CommandQueue*> queues{};
        HashMap<int, Array<CommandQueue*>> queuesByFamily{};
    };
    
	FrameGraphCompiler::FrameGraphCompiler(VulkanDevice* device) : device(device)
	{

	}

	FrameGraphCompiler::~FrameGraphCompiler()
	{
		vkDeviceWaitIdle(device->GetHandle());

		DestroySyncObjects();
		DestroyCommandLists();
	}

	void FrameGraphCompiler::CompileScopesInternal(const FrameGraphCompileRequest& compileRequest)
	{
		vkDeviceWaitIdle(device->GetHandle());

		FrameGraph* frameGraph = compileRequest.frameGraph;
        QueueAllocator queueAllocator{};
        queueAllocator.queuesByFamily = device->queuesByFamily;
        queueAllocator.presentQueue = device->presentQueue;
        queueAllocator.primaryQueue = device->primaryGraphicsQueue;
        queueAllocator.queues = device->queues;
        queueAllocator.Init();
		
        std::function<void(RHI::Scope*, int)> visitor = [&](RHI::Scope* rhiScope, int i)
        {
            Vulkan::Scope* scope = (Vulkan::Scope*)rhiScope;
            if (scope->queue == nullptr)
			{
				scope->queue = queueAllocator.Acquire(i, scope->queueClass, scope->PresentsSwapChain());
			}

			for (auto attachment : scope->attachments)
			{
				FrameAttachment* frameAttachment = attachment->GetFrameAttachment();
				scope->usesSwapChainAttachment = frameAttachment->IsSwapChainAttachment();
			}
            
            for (auto consumer : frameGraph->nodes[scope->id].consumers)
            {
				visitor(consumer, i);
				i++;
            }
        };
        
        int trackNumber = 0;
        for (RHI::Scope* scope : frameGraph->producers)
        {
			visitor(scope, trackNumber);
			trackNumber++;
        }
	}

	void FrameGraphCompiler::CompileInternal(const FrameGraphCompileRequest& compileRequest)
	{
		FrameGraph* frameGraph = compileRequest.frameGraph;

		vkDeviceWaitIdle(device->GetHandle());

		Vulkan::Scope* presentingScope = (Vulkan::Scope*)frameGraph->presentingScope;
		auto swapChain = (Vulkan::SwapChain*)frameGraph->presentSwapChain;
		numFramesInFlight = compileRequest.numFramesInFlight;
		imageCount = numFramesInFlight;

		if (swapChain && presentingScope)
		{
			imageCount = swapChain->GetImageCount();
			numFramesInFlight = Math::Min<u32>(imageCount - 1, numFramesInFlight);
		}
		
		// Compile sync objects for individual scopes
		for (auto scope : frameGraph->scopes)
		{
			scope->Compile(compileRequest);
		}

		for (auto& fences : graphExecutionFences)
		{
			fences.Clear();
		}

		for (auto rhiScope : frameGraph->endScopes)
		{
			Vulkan::Scope* scope = (Vulkan::Scope*)rhiScope;
			for (int i = 0; i < imageCount; i++)
			{
				graphExecutionFences[i].Add(scope->renderFinishedFences[i]);
			}
		}

		for (auto scope : frameGraph->producers)
		{
			CompileCrossQueueDependencies(compileRequest, (Vulkan::Scope*)scope);
		}

		// ---------------------------
		// Destroy old objects

		DestroySyncObjects();
		DestroyCommandLists();

		// ----------------------------
		// Create new objects

		if (swapChain && presentingScope)
		{
			imageCount = swapChain->GetImageCount();
			numFramesInFlight = Math::Min<u32>(imageCount - 1, numFramesInFlight);

			for (int i = 0; i < imageCount; i++)
			{
				VkSemaphoreCreateInfo semaphoreCI{};
				semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

				VkSemaphore semaphore = nullptr;
				auto result = vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &semaphore);
				if (result != VK_SUCCESS)
				{
					continue;
				}

				imageAcquiredSemaphores.Add(semaphore);

				VkFenceCreateInfo fenceCI{};
				fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;

				/*VkFence fence = nullptr;
				result = vkCreateFence(device->GetHandle(), &fenceCI, nullptr, &fence);
				if (result != VK_SUCCESS)
				{
					continue;
				}

				imageAcquiredFences.Add(fence);*/
			}
		}

		for (int imageIdx = 0; imageIdx < imageCount; imageIdx++)
		{
			commandListsByImageIndex.Add({});

			for (u32 familyIdx = 0; familyIdx < device->queueFamilyPropeties.GetSize(); familyIdx++)
			{
				VkCommandBuffer cmdBuffer = nullptr;
				VkCommandPool pool = device->AllocateCommandBuffers(1, &cmdBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, familyIdx);
				CommandList* commandList = new Vulkan::CommandList(device, cmdBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, familyIdx, pool);
				commandListsByImageIndex[imageIdx].Add(commandList);
			}
		}

		// Compile barriers
		CompileBarriers(compileRequest);

	}

    void FrameGraphCompiler::DestroySyncObjects()
    {
		vkDeviceWaitIdle(device->GetHandle());

		for (VkSemaphore semaphore : imageAcquiredSemaphores)
		{
			vkDestroySemaphore(device->GetHandle(), semaphore, nullptr);
		}
		imageAcquiredSemaphores.Clear();
    }

	void FrameGraphCompiler::DestroyCommandLists()
	{
		for (Array<CommandList*>& commandLists : commandListsByImageIndex)
		{
			for (CommandList* commandList : commandLists)
			{
				delete commandList;
			}
			commandLists.Clear();
		}
		commandListsByImageIndex.Clear();
	}

    //! If two scopes are executed one different queues and there's a dependency between them, then we
	//! need to use a wait semaphore for it.
    void FrameGraphCompiler::CompileCrossQueueDependencies(const FrameGraphCompileRequest& compileRequest, Vulkan::Scope* current)
	{
		FrameGraph* frameGraph = compileRequest.frameGraph;

		const auto& producers = current->producers;
		
		for (RHI::Scope* rhiScope : producers)
		{
			Vulkan::Scope* producerScope = (Vulkan::Scope*)rhiScope;

			HashMap<ScopeAttachment*, ScopeAttachment*> commonAttachments = Scope::FindCommonFrameAttachments(producerScope, current);

			for (auto [from, to] : commonAttachments)
			{
				VkPipelineStageFlags flags{};

				if (to->GetUsage() == RHI::ScopeAttachmentUsage::RenderTarget)
				{
					flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				}
				else if (to->GetUsage() == RHI::ScopeAttachmentUsage::DepthStencil)
				{
					flags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				}
				else if (to->GetUsage() == RHI::ScopeAttachmentUsage::SubpassInput || to->GetUsage() == RHI::ScopeAttachmentUsage::Shader)
				{
					flags = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
				}
				else
				{
					flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				}

				for (int i = 0; i < imageCount; i++)
				{
					current->waitSemaphores[i].Add(producerScope->renderFinishedSemaphores[i]);
				}
				current->waitSemaphoreStageFlags.Add(flags);
			}
		}
		
		for (RHI::Scope* consumer : current->consumers)
		{
			CompileCrossQueueDependencies(compileRequest, (Vulkan::Scope*)consumer);
		}
	}

	void FrameGraphCompiler::CompileBarriers(const FrameGraphCompileRequest& compileRequest)
	{
		FrameGraph* frameGraph = compileRequest.frameGraph;

		for (int i = 0; i < imageCount; i++)
		{
			for (RHI::Scope* scope : frameGraph->producers)
			{
				CompileBarriers(i, compileRequest, (Vulkan::Scope*)scope);
			}
		}
	}

	static bool RequiresDependency(ScopeAttachment* from, ScopeAttachment* to)
	{
		if (from == nullptr || to == nullptr)
			return false;

		if (EnumHasFlag(to->GetAccess(), RHI::ScopeAttachmentAccess::Write) ||
			EnumHasFlag(from->GetAccess(), RHI::ScopeAttachmentAccess::Write))
			return true;

		switch (to->GetUsage())
		{
		case RHI::ScopeAttachmentUsage::RenderTarget:
		case RHI::ScopeAttachmentUsage::Resolve:
			return true;
		}

		switch (from->GetUsage())
		{
		case RHI::ScopeAttachmentUsage::RenderTarget:
		case RHI::ScopeAttachmentUsage::Resolve:
			return true;
		}

		return false;
	}

	void FrameGraphCompiler::CompileBarriers(int imageIndex, const FrameGraphCompileRequest& compileRequest, Vulkan::Scope* current)
	{
		if (current == nullptr)
			return;

		FrameGraph* frameGraph = compileRequest.frameGraph;
		Vulkan::Scope* presentingScope = (Vulkan::Scope*)frameGraph->presentingScope;
		auto swapChain = (Vulkan::SwapChain*)frameGraph->presentSwapChain;

		for (RHI::Scope* producerRhiScope : current->producers)
		{
			Vulkan::Scope* producerScope = (Vulkan::Scope*)producerRhiScope;
			if (producerScope->queue != current->queue)
			{
				current->barriers[imageIndex].Clear();
				break;
			}

			HashMap<ScopeAttachment*, ScopeAttachment*> commonAttachments = Scope::FindCommonFrameAttachments(producerRhiScope, current);

			for (auto [from, to] : commonAttachments)
			{
				Scope::Barrier barrier{};
				
				// Image -> Image barrier
				if (from->IsImageAttachment() && to->IsImageAttachment() && RequiresDependency(from, to))
				{
					ImageScopeAttachment* fromImage = (ImageScopeAttachment*)from;
					ImageScopeAttachment* toImage = (ImageScopeAttachment*)to;
					VkImageMemoryBarrier imageBarrier{};
					imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

					ImageFrameAttachment* imageAttachment = (ImageFrameAttachment*)fromImage->GetFrameAttachment();
					if (imageAttachment == nullptr)
						continue;

					RHIResource* resource = imageAttachment->GetResource(imageIndex);
					if (resource == nullptr || resource->GetResourceType() != RHI::ResourceType::Texture)
						continue;

					Vulkan::Texture* image = dynamic_cast<Vulkan::Texture*>(resource);
					if (image == nullptr || image->GetImage() == nullptr)
						continue;

					imageBarrier.image = image->GetImage();
					imageBarrier.srcQueueFamilyIndex = producerScope->queue->GetFamilyIndex();
					imageBarrier.dstQueueFamilyIndex = current->queue->GetFamilyIndex();

					imageBarrier.subresourceRange.aspectMask = image->GetAspectMask();
					imageBarrier.subresourceRange.baseArrayLayer = 0;
					imageBarrier.subresourceRange.layerCount = image->GetArrayLayerCount();
					imageBarrier.subresourceRange.baseMipLevel = 0;
					imageBarrier.subresourceRange.levelCount = image->GetMipLevelCount();

					switch (fromImage->GetUsage())
					{
					case RHI::ScopeAttachmentUsage::RenderTarget:
						barrier.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
						imageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
						imageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
						break;
					case RHI::ScopeAttachmentUsage::DepthStencil:
						barrier.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
						imageBarrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
						imageBarrier.srcAccessMask = 0;
						if (EnumHasFlag(fromImage->GetAccess(), RHI::ScopeAttachmentAccess::Read))
							imageBarrier.srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
						if (EnumHasFlag(fromImage->GetAccess(), RHI::ScopeAttachmentAccess::Write))
						{
							imageBarrier.srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
						}
						else // Read only
						{
							imageBarrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
						}
						break;
					case RHI::ScopeAttachmentUsage::SubpassInput:
					case RHI::ScopeAttachmentUsage::Shader:
						barrier.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
						imageBarrier.srcAccessMask = 0;
						if (EnumHasFlag(fromImage->GetAccess(), RHI::ScopeAttachmentAccess::Read))
							imageBarrier.srcAccessMask |= VK_ACCESS_SHADER_READ_BIT;
						if (EnumHasFlag(fromImage->GetAccess(), RHI::ScopeAttachmentAccess::Write))
						{
							imageBarrier.srcAccessMask |= VK_ACCESS_SHADER_WRITE_BIT;
							imageBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
						}
						else // Read only
						{
							imageBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						}
						break;
					case RHI::ScopeAttachmentUsage::Copy:
						barrier.srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
						imageBarrier.srcAccessMask = 0;
						if (EnumHasFlag(fromImage->GetAccess(), RHI::ScopeAttachmentAccess::Read))
						{
							imageBarrier.srcAccessMask |= VK_ACCESS_MEMORY_READ_BIT;
							imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
						}
						else if (EnumHasFlag(fromImage->GetAccess(), RHI::ScopeAttachmentAccess::Write))
						{
							imageBarrier.srcAccessMask |= VK_ACCESS_MEMORY_WRITE_BIT;
							imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
						}
						break;
					case RHI::ScopeAttachmentUsage::Resolve:
						barrier.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
						imageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
						imageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
						break;
					default:
						continue;
					}

					switch (toImage->GetUsage())
					{
					case RHI::ScopeAttachmentUsage::RenderTarget:
						barrier.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
						imageBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
						imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
						break;
					case RHI::ScopeAttachmentUsage::DepthStencil:
						barrier.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
						imageBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
						imageBarrier.dstAccessMask = 0;
						if (EnumHasFlag(toImage->GetAccess(), RHI::ScopeAttachmentAccess::Read))
							imageBarrier.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
						if (EnumHasFlag(toImage->GetAccess(), RHI::ScopeAttachmentAccess::Write))
						{
							imageBarrier.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
						}
						else // Read only
						{
							imageBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
						}
						break;
					case RHI::ScopeAttachmentUsage::SubpassInput:
					case RHI::ScopeAttachmentUsage::Shader:
						barrier.dstStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
						imageBarrier.dstAccessMask = 0;
						if (EnumHasFlag(toImage->GetAccess(), RHI::ScopeAttachmentAccess::Read))
							imageBarrier.dstAccessMask |= VK_ACCESS_SHADER_READ_BIT;
						if (EnumHasFlag(toImage->GetAccess(), RHI::ScopeAttachmentAccess::Write))
						{
							imageBarrier.dstAccessMask |= VK_ACCESS_SHADER_WRITE_BIT;
							imageBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
						}
						else // Read only
						{
							imageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						}
						break;
					case RHI::ScopeAttachmentUsage::Copy:
						barrier.dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
						imageBarrier.dstAccessMask = 0;
						if (EnumHasFlag(fromImage->GetAccess(), RHI::ScopeAttachmentAccess::Read))
						{
							imageBarrier.dstAccessMask |= VK_ACCESS_MEMORY_READ_BIT;
							imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
						}
						else if (EnumHasFlag(fromImage->GetAccess(), RHI::ScopeAttachmentAccess::Write))
						{
							imageBarrier.dstAccessMask |= VK_ACCESS_MEMORY_WRITE_BIT;
							imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
						}
						break;
					case RHI::ScopeAttachmentUsage::Resolve:
						barrier.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
						imageBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
						imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
						break;
					default:
						continue;
					}

					Scope::ImageLayoutTransition transition{};
					transition.image = image;
					transition.layout = imageBarrier.newLayout;
					transition.queueFamilyIndex = current->queue->GetFamilyIndex();

					barrier.imageBarriers.Add(imageBarrier);
					barrier.imageLayoutTransitions.Add(transition);
				}
				// Buffer -> Buffer barrier
				else if (from->IsBufferAttachment() && to->IsBufferAttachment() && RequiresDependency(from, to))
				{
					BufferScopeAttachment* fromBuffer = (BufferScopeAttachment*)from;
					BufferScopeAttachment* toBuffer = (BufferScopeAttachment*)to;
					VkBufferMemoryBarrier bufferBarrier{};
					bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;

					BufferFrameAttachment* bufferAttachment = (BufferFrameAttachment*)fromBuffer->GetFrameAttachment();
					if (bufferAttachment == nullptr)
						continue;

					RHIResource* resource = bufferAttachment->GetResource(imageIndex);
					if (resource == nullptr || resource->GetResourceType() != RHI::ResourceType::Buffer)
						continue;

					Vulkan::Buffer* buffer = dynamic_cast<Vulkan::Buffer*>(resource);
					if (buffer == nullptr || buffer->GetBuffer() == nullptr)
						continue;
					
					bufferBarrier.buffer = buffer->GetBuffer();
					bufferBarrier.srcQueueFamilyIndex = producerScope->queue->GetFamilyIndex();
					bufferBarrier.dstQueueFamilyIndex = current->queue->GetFamilyIndex();
                    bufferBarrier.offset = 0;
                    bufferBarrier.size = buffer->GetBufferSize();

					switch (fromBuffer->GetUsage())
					{
					case RHI::ScopeAttachmentUsage::Copy:
					case RHI::ScopeAttachmentUsage::Shader:
						if (EnumHasFlag(fromBuffer->GetAccess(), RHI::ScopeAttachmentAccess::Read))
						{
							bufferBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
						}
						else if (EnumHasFlag(fromBuffer->GetAccess(), RHI::ScopeAttachmentAccess::Write))
						{
							bufferBarrier.srcAccessMask |= VK_ACCESS_MEMORY_WRITE_BIT;
						}
						break;
					default:
						continue;
					}

					switch (toBuffer->GetUsage())
					{
					case RHI::ScopeAttachmentUsage::Copy:
					case RHI::ScopeAttachmentUsage::Shader:
						if (EnumHasFlag(toBuffer->GetAccess(), RHI::ScopeAttachmentAccess::Read))
						{
							bufferBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
						}
						else if (EnumHasFlag(toBuffer->GetAccess(), RHI::ScopeAttachmentAccess::Write))
						{
							bufferBarrier.dstAccessMask |= VK_ACCESS_MEMORY_WRITE_BIT;
						}
						break;
					default:
						continue;
					}

					barrier.bufferBarriers.Add(bufferBarrier);
				}
				else
				{
					continue;
				}

				current->barriers[imageIndex].Add(barrier);
			}
		}

		for (auto scope : current->consumers)
		{
			CompileBarriers(imageIndex, compileRequest, (Vulkan::Scope*)scope);
		}
	}

} // namespace CE::Vulkan
