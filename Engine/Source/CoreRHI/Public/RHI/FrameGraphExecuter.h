#pragma once

namespace CE::RHI
{
	class FrameGraph;
	struct FrameGraphCompileRequest;
	class FrameGraphCompiler;
	class FrameScheduler;

	struct FrameGraphExecuteRequest
	{
		FrameGraph* frameGraph = nullptr;
		FrameScheduler* scheduler = nullptr;
		FrameGraphCompiler* compiler = nullptr;
	};
    
	class CORERHI_API FrameGraphExecuter
	{
	public:
		virtual ~FrameGraphExecuter();

		bool Execute(const FrameGraphExecuteRequest& executeRequest);

		virtual void WaitUntilIdle() = 0;

		virtual u32 BeginExecution(const FrameGraphExecuteRequest& executeRequest) = 0;

		virtual void EndExecution(const FrameGraphExecuteRequest& executeRequest) = 0;

		virtual void ResetFramesInFlight() = 0;

	protected:
		FrameGraphExecuter() = default;

	private:

		virtual bool ExecuteInternal(const FrameGraphExecuteRequest& executeRequest) = 0;

	};

} // namespace CE::RHI
