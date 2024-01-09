#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

    class QueueAllocator
    {
    public:
        
        void Init()
        {
            for (const auto& [familyIdx, queues] : queuesByFamily)
            {
                
            }
        }
        
        CommandQueue* Acquire(int index, RHI::HardwareQueueClass queueClass, bool present = false)
        {
            return primaryQueue;
        }
        
        int graphicsQueuesAvailable = 0;
        int computeQueuesAvailable = 0;
        int transferQueuesAvailable = 0;
        
        CommandQueue* primaryQueue = nullptr;
        Array<VkQueueFlags> queueFamilyFlags{};
        Array<CommandQueue*> presentQueues{};
        Array<CommandQueue*> queues{};
        HashMap<int, Array<CommandQueue*>> queuesByFamily{};
    };
    
	FrameGraphCompiler::FrameGraphCompiler(VulkanDevice* device) : device(device)
	{

	}

	FrameGraphCompiler::~FrameGraphCompiler()
	{

	}

	void FrameGraphCompiler::CompileCrossQueueScopes(const FrameGraphCompilerRequest& compileRequest)
	{
		FrameGraph* frameGraph = compileRequest.frameGraph;
        QueueAllocator queueAllocator{};
        for (auto familyProps : device->queueFamilyPropeties)
        {
            queueAllocator.queueFamilyFlags.Add(familyProps.queueFlags);
        }
        queueAllocator.queuesByFamily = device->queuesByFamily;
        queueAllocator.presentQueues = device->presentQueue;
        queueAllocator.primaryQueue = device->primaryGraphicsQueue;
        queueAllocator.queues = device->queues;
        queueAllocator.Init();
        
        // TODO: Allocate proper queues
		
        std::function<void(RHI::Scope*, int)> func = [&](RHI::Scope* rhiScope, int i)
        {
            Vulkan::Scope* scope = (Vulkan::Scope*)rhiScope;
            if (scope->queue == nullptr)
                scope->queue = queueAllocator.Acquire(i, scope->queueClass);
            
            for (auto consumer : frameGraph->nodes[scope->id].consumers)
            {
                func(consumer, i++);
            }
        };
        
        int i = 0;
        for (auto scope : frameGraph->producers)
        {
            func(scope, i);
            i++;
        }
	}

	void FrameGraphCompiler::CompileInternal(const FrameGraphCompilerRequest& compileRequest)
	{

	}

} // namespace CE::Vulkan
