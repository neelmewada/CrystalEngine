#include "CoreRHI.h"

namespace CE::RHI
{
	FrameScheduler::FrameScheduler(const FrameSchedulerDescriptor& descriptor)
	{
		frameGraph = new FrameGraph();
        transientMemoryPool = new TransientMemoryPool();
		numFramesInFlight = descriptor.numFramesInFlight;

		compiler = RHI::gDynamicRHI->CreateFrameGraphCompiler();
	}

	FrameScheduler::~FrameScheduler()
	{
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
        FrameGraphCompileRequest compileRequest{};
        compileRequest.frameGraph = frameGraph;
        compileRequest.transientPool = transientMemoryPool;
		compileRequest.numFramesInFlight = 1;

        compiler->Compile(compileRequest);
    }

	void FrameScheduler::BeginDrawListSubmission()
	{
		for (RHI::Scope* scope : frameGraph->scopes)
		{
			scope->drawList.Clear();
		}
	}

	void FrameScheduler::BeginDrawListScope(ScopeID scopeId)
	{
		RHI::Scope* scope = frameGraph->scopesById[scopeId];
		if (scope != nullptr)
		{
			drawListScope = scope;
			drawListScope->drawList.Clear();
		}
		else
		{
			frameGraph->scopesById.Remove(scopeId);
		}
	}

	void FrameScheduler::EndDrawListScope()
	{
		drawListScope = nullptr;
	}

	void FrameScheduler::EndDrawListSubmission()
	{

	}

	void FrameScheduler::Execute(const FrameGraphExecuteRequest& executeRequest)
	{

	}

    FrameAttachment* FrameScheduler::GetFrameAttachment(AttachmentID id) const
    {
        return GetAttachmentDatabase().FindFrameAttachment(id);
    }

} // namespace CE::RHI
