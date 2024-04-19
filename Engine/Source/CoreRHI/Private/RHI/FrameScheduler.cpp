#include "CoreRHI.h"

namespace CE::RHI
{
	FrameScheduler* frameSchedulerInstance = nullptr;

	FrameScheduler::FrameScheduler(const FrameSchedulerDescriptor& descriptor)
	{
		frameSchedulerInstance = this;

		frameGraph = new FrameGraph();
        transientMemoryPool = new TransientMemoryPool();
		numFramesInFlight = descriptor.numFramesInFlight;

		compiler = RHI::gDynamicRHI->CreateFrameGraphCompiler();
		executer = RHI::gDynamicRHI->CreateFrameGraphExecuter();
	}

	FrameScheduler* FrameScheduler::Create(const FrameSchedulerDescriptor& descriptor)
	{
		return new FrameScheduler(descriptor);
	}

	FrameScheduler::~FrameScheduler()
	{
		if (frameSchedulerInstance == this)
		{
			frameSchedulerInstance = nullptr;
		}

		delete executer;
		delete compiler;
        delete transientMemoryPool;
		delete frameGraph;
	}

	void FrameScheduler::BeginFrameGraph()
	{
		GetAttachmentDatabase().Clear();

		FrameGraphBuilder::BeginFrameGraph(frameGraph);
	}

    void FrameScheduler::Compile()
    {
		if (frameGraph->presentSwapChains.NonEmpty())
		{
			numFramesInFlight = frameGraph->presentSwapChains[0]->GetImageCount();
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
		executeRequest.scheduler = this;
		executeRequest.compiler = compiler;

		return executer->BeginExecution(executeRequest);
	}

	void FrameScheduler::EndExecution()
	{
		FrameGraphExecuteRequest executeRequest{};
		executeRequest.frameGraph = frameGraph;
		executeRequest.scheduler = this;
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

	FrameScheduler* FrameScheduler::Get()
	{
		return frameSchedulerInstance;
	}

	void FrameScheduler::SetFrameGraphVariable(const Name& variableName, const RHI::FrameGraphVariable& value)
	{
		frameGraph->SetVariable(variableName, value);
	}

	void FrameScheduler::SetFrameGraphVariable(int imageIndex, const Name& variableName, const RHI::FrameGraphVariable& value)
	{
		frameGraph->SetVariable(imageIndex, variableName, value);
	}

} // namespace CE::RHI
