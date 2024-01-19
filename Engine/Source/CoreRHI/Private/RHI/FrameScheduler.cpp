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

	void FrameScheduler::BeginDrawListSubmission()
	{

	}

	void FrameScheduler::BeginDrawListScope(ScopeID scopeId)
	{
		RHI::Scope* scope = frameGraph->scopesById[scopeId];
		if (scope != nullptr)
		{
			drawListScope = scope;
			drawListScope->drawList.Clear();
			drawListScope->threadDrawLists.Clear();
		}
		else
		{
			frameGraph->scopesById.Remove(scopeId);
		}
	}

	void FrameScheduler::SubmitDrawItem(DrawItemProperties drawItemProperties)
	{
		if (drawListScope != nullptr)
		{
			drawListScope->threadDrawLists.GetStorage().AddDrawItem(drawItemProperties);
		}
	}

	void FrameScheduler::EndDrawListScope()
	{
		if (drawListScope != nullptr)
		{
			drawListScope->drawList.Clear();
			drawListScope->threadDrawLists.ForEach([&](RHI::DrawList& drawList)
				{
					drawListScope->drawList.Merge(drawList);
				});
			drawListScope->threadDrawLists.Clear();
		}
		drawListScope = nullptr;
	}

	void FrameScheduler::EndDrawListSubmission()
	{

	}

	void FrameScheduler::Execute()
	{
		FrameGraphExecuteRequest executeRequest{};
		executeRequest.frameGraph = frameGraph;
		executeRequest.compiler = compiler;

		executer->Execute(executeRequest);
	}

    FrameAttachment* FrameScheduler::GetFrameAttachment(AttachmentID id) const
    {
        return GetAttachmentDatabase().FindFrameAttachment(id);
    }

} // namespace CE::RHI
