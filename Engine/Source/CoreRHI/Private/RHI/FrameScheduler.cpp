#include "CoreRHI.h"

namespace CE::RHI
{
	FrameScheduler::FrameScheduler(const FrameSchedulerDescriptor& descriptor)
	{
		frameGraph = new FrameGraph();
        transientMemoryPool = new TransientMemoryPool();
		numFramesInFlight = descriptor.numFramesInFlight;

		compiler = RHI::gDynamicRHI->CreateFrameGraphCompiler();
		executer = RHI::gDynamicRHI->CreateFrameGraphExecuter();
	}

	FrameScheduler::~FrameScheduler()
	{
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

	void FrameScheduler::SetScopeDrawList(const ScopeID& scopeId, DrawListContext* drawList)
	{
		if (!frameGraph->scopesById.KeyExists(scopeId))
			return;

		Scope* scope = frameGraph->scopesById[scopeId];
		if (!scope)
			return;

		scope->drawList = drawList;
	}

    FrameAttachment* FrameScheduler::GetFrameAttachment(AttachmentID id) const
    {
        return GetAttachmentDatabase().FindFrameAttachment(id);
    }

} // namespace CE::RHI
