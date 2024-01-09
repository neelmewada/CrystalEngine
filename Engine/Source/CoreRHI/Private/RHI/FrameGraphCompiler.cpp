#include "CoreRHI.h"

namespace CE::RHI
{

	void FrameGraphCompiler::Compile(const FrameGraphCompilerRequest& compileRequest)
	{
		// 1. Compile cross queue scope
		CompileCrossQueueScopes(compileRequest);
	}

} // namespace CE::RHI
