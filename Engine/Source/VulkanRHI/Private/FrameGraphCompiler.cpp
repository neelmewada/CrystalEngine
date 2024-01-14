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
		DestroySyncObjects();
		DestroyCommandLists();
	}

	void FrameGraphCompiler::CompileScopesInternal(const FrameGraphCompileRequest& compileRequest)
	{
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
                scope->queue = queueAllocator.Acquire(i, scope->queueClass, scope->PresentsSwapChain());
            
            for (auto consumer : frameGraph->nodes[scope->id].consumers)
            {
				visitor(consumer, i);
				i++;
            }
        };
        
        int trackNumber = 0;
        for (auto scope : frameGraph->producers)
        {
			visitor(scope, trackNumber);
			trackNumber++;
        }
	}

	void FrameGraphCompiler::CompileInternal(const FrameGraphCompileRequest& compileRequest)
	{
		FrameGraph* frameGraph = compileRequest.frameGraph;

		vkDeviceWaitIdle(device->GetHandle());
		
		// Compile sync objects for individual scopes
		for (auto scope : frameGraph->scopes)
		{
			scope->Compile(compileRequest);
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

		Vulkan::Scope* presentingScope = (Vulkan::Scope*)frameGraph->presentingScope;
		auto swapChain = (Vulkan::SwapChain*)frameGraph->presentSwapChain;
		numFramesInFlight = compileRequest.numFramesInFlight;
		imageCount = numFramesInFlight;

		if (swapChain && presentingScope)
		{
			imageCount = swapChain->GetImageCount();
			numFramesInFlight = Math::Min<u32>(imageCount - 1, numFramesInFlight);

			for (int i = 0; i < numFramesInFlight; i++)
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

				VkFence fence = nullptr;
				result = vkCreateFence(device->GetHandle(), &fenceCI, nullptr, &fence);
				if (result != VK_SUCCESS)
				{
					continue;
				}

				imageAcquiredFences.Add(fence);
			}
		}

		for (int i = 0; i < imageCount; i++)
		{
			VkSemaphoreCreateInfo semaphoreCI{};
			semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			VkSemaphore semaphore = nullptr;
			vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &semaphore);
			graphExecutedSemaphores.Add(semaphore);

			VkFenceCreateInfo fenceCI{};
			fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			VkFence fence = nullptr;
			vkCreateFence(device->GetHandle(), &fenceCI, nullptr, &fence);
			graphExecutedFences.Add(fence);

			commandListsByImageIndex.Add({});

			for (u32 familyIdx = 0; familyIdx < device->queueFamilyPropeties.GetSize(); familyIdx++)
			{
				VkCommandBuffer cmdBuffer = nullptr;
				VkCommandPool pool = device->AllocateCommandBuffers(1, &cmdBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, familyIdx);
				CommandList* commandList = new Vulkan::CommandList(device, cmdBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, familyIdx, pool);
				commandListsByImageIndex[i].Add(commandList);
			}
		}

	}

    void FrameGraphCompiler::DestroySyncObjects()
    {
		for (VkSemaphore semaphore : imageAcquiredSemaphores)
		{
			vkDestroySemaphore(device->GetHandle(), semaphore, nullptr);
		}
		imageAcquiredSemaphores.Clear();

		for (VkFence fence : imageAcquiredFences)
		{
			vkDestroyFence(device->GetHandle(), fence, nullptr);
		}
		imageAcquiredFences.Clear();

		for (VkSemaphore semaphore : graphExecutedSemaphores)
		{
			vkDestroySemaphore(device->GetHandle(), semaphore, nullptr);
		}
		graphExecutedSemaphores.Clear();

		for (VkFence fence : graphExecutedFences)
		{
			vkDestroyFence(device->GetHandle(), fence, nullptr);
		}
		graphExecutedFences.Clear();
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


	}

} // namespace CE::Vulkan
