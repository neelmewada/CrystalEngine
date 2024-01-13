#include "CoreRHI.h"

namespace CE::RHI
{

    FrameGraphExecuter::~FrameGraphExecuter()
    {

    }

	bool FrameGraphExecuter::Execute(const FrameGraphExecuteRequest& executeRequest)
	{
		return ExecuteInternal(executeRequest);
	}

	bool FrameGraphExecuter::Compile(const FrameGraphCompileRequest& compileRequest)
	{
		return CompileInternal(compileRequest);
	}

} // namespace CE::RHI
