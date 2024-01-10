#include "FrameGraphCompiler.h"
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

	}

	void FrameGraphCompiler::CompileCrossQueueScopes(const FrameGraphCompileRequest& compileRequest)
	{
		FrameGraph* frameGraph = compileRequest.frameGraph;
        QueueAllocator queueAllocator{};
        queueAllocator.queuesByFamily = device->queuesByFamily;
        queueAllocator.presentQueue = device->presentQueue;
        queueAllocator.primaryQueue = device->primaryGraphicsQueue;
        queueAllocator.queues = device->queues;
        queueAllocator.Init();
		
        std::function<void(RHI::Scope*, int)> func = [&](RHI::Scope* rhiScope, int i)
        {
            Vulkan::Scope* scope = (Vulkan::Scope*)rhiScope;
            if (scope->queue == nullptr)
                scope->queue = queueAllocator.Acquire(i, scope->queueClass, scope->PresentsSwapChain());
            
            for (auto consumer : frameGraph->nodes[scope->id].consumers)
            {
                func(consumer, i);
				i++;
            }
        };
        
        int trackNumber = 0;
        for (auto scope : frameGraph->producers)
        {
            func(scope, trackNumber);
			trackNumber++;
        }
	}

	void FrameGraphCompiler::CompileInternal(const FrameGraphCompileRequest& compileRequest)
	{

	}

} // namespace CE::Vulkan
