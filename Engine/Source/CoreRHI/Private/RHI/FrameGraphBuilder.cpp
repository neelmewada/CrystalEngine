#include "CoreRHI.h"

namespace CE::RHI
{

    void FrameGraphBuilder::Begin(FrameGraph* frameGraph)
    {
		this->frameGraph = frameGraph;
    }

	void FrameGraphBuilder::BeginScope()
	{
		this->currentScope = RHI::gDynamicRHI->CreateScope({});
	}

	bool FrameGraphBuilder::EndScope()
	{
		if (!currentScope || !frameGraph)
			return false;
		frameGraph->scopes.Add(currentScope);
		return true;
	}

    bool FrameGraphBuilder::End()
    {
		return true;
    }

} // namespace CE::RHI
