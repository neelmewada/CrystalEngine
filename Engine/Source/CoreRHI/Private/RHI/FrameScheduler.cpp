#include "CoreRHI.h"

namespace CE::RHI
{
	FrameScheduler::FrameScheduler(const FrameSchedulerDescriptor& descriptor)
	{
		frameGraph = new FrameGraph();
        transientMemoryPool = new TransientMemoryPool();
	}

	FrameScheduler::~FrameScheduler()
	{
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
        
        FrameGraphCompiler compiler{};
        compiler.Compile(compileRequest);
    }

    void FrameScheduler::BeginFrame()
	{
		
	}

	void FrameScheduler::EndFrame()
	{

	}

    FrameAttachment* FrameScheduler::GetFrameAttachment(AttachmentID id)
    {
        return GetAttachmentDatabase().FindFrameAttachment(id);
    }

} // namespace CE::RHI
