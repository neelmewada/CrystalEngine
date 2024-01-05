#include "CoreRHI.h"

namespace CE::RHI
{

    FrameGraph::FrameGraph()
    {
        
    }

    FrameGraph::~FrameGraph()
    {
		for (auto scope : scopes)
		{
			delete scope;
		}
    }
    
} // namespace CE::RHI
