#include "CoreRHI.h"

namespace CE::RHI
{
	SharedMutex frameSchedulerInstancesMutex{};
	Array<FrameScheduler*> FrameScheduler::frameSchedulerInstances{};

	FrameScheduler::FrameScheduler(const FrameSchedulerDescriptor& descriptor)
	{
		frameSchedulerInstances.Add(this);

		frameGraph = new FrameGraph();
        transientMemoryPool = new TransientMemoryPool();
		numFramesInFlight = descriptor.numFramesInFlight;

		compiler = RHI::gDynamicRHI->CreateFrameGraphCompiler();
		executer = RHI::gDynamicRHI->CreateFrameGraphExecuter();
	}

	FrameScheduler* FrameScheduler::Create(const FrameSchedulerDescriptor& descriptor)
	{
		LockGuard<SharedMutex> lock{ frameSchedulerInstancesMutex };

		if (frameSchedulerInstances.GetSize() >= RHI::Limits::MaxFrameSchedulerCount)
			return nullptr;

		return new FrameScheduler(descriptor);
	}

	FrameScheduler::~FrameScheduler()
	{
		LockGuard<SharedMutex> lock{ frameSchedulerInstancesMutex };

		frameSchedulerInstances.Remove(this);

		delete executer;
		delete compiler;
        delete transientMemoryPool;
		delete frameGraph;
	}

	void FrameScheduler::BeginFrameGraph()
	{
		FrameGraphBuilder::BeginFrameGraph(frameGraph);
	}

	void FrameScheduler::Construct()
	{
		
	}

    void FrameScheduler::Compile()
    {
		if (frameGraph->presentSwapChain != nullptr)
		{
			numFramesInFlight = frameGraph->presentSwapChain->GetImageCount();
		}

        FrameGraphCompileRequest compileRequest{};
        compileRequest.frameGraph = frameGraph;
        compileRequest.transientPool = transientMemoryPool;
		compileRequest.numFramesInFlight = numFramesInFlight;
		compileRequest.shrinkPool = true;
		
        compiler->Compile(compileRequest);
    }

	void FrameScheduler::Execute()
	{
		FrameGraphExecuteRequest executeRequest{};
		executeRequest.frameGraph = frameGraph;
		executeRequest.compiler = compiler;

		executer->Execute(executeRequest);
	}

	u32 FrameScheduler::BeginExecution()
	{
		FrameGraphExecuteRequest executeRequest{};
		executeRequest.frameGraph = frameGraph;
		executeRequest.compiler = compiler;

		return executer->BeginExecution(executeRequest);
	}

	void FrameScheduler::EndExecution()
	{
		FrameGraphExecuteRequest executeRequest{};
		executeRequest.frameGraph = frameGraph;
		executeRequest.compiler = compiler;

		executer->EndExecution(executeRequest);
	}

	void FrameScheduler::SetScopeDrawList(const ScopeID& scopeId, DrawList* drawList)
	{
		if (!frameGraph->scopesById.KeyExists(scopeId))
			return;

		Scope* scope = frameGraph->scopesById[scopeId];
		if (!scope)
			return;

		scope->drawList = drawList;
	}

	RHI::Scope* FrameScheduler::FindScope(const ScopeID& scopeId)
	{
		return frameGraph->scopesById[scopeId];
	}

    FrameAttachment* FrameScheduler::GetFrameAttachment(AttachmentID id) const
    {
        return GetAttachmentDatabase().FindFrameAttachment(id);
    }

	void FrameScheduler::WaitUntilIdle()
	{
		executer->WaitUntilIdle();
	}

	FrameScheduler* FrameScheduler::GetFrameScheduler(int instanceIndex)
	{
		LockGuard<SharedMutex> lock{ frameSchedulerInstancesMutex };

		if (instanceIndex < 0 || instanceIndex >= frameSchedulerInstances.GetSize())
			return nullptr;
		return frameSchedulerInstances[instanceIndex];
	}

	int FrameScheduler::GetTotalFrameSchedulerCount()
	{
		LockGuard<SharedMutex> lock{ frameSchedulerInstancesMutex };

		return frameSchedulerInstances.GetSize();
	}

} // namespace CE::RHI
