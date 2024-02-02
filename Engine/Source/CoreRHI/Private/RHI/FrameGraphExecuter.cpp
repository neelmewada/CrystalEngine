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

} // namespace CE::RHI
