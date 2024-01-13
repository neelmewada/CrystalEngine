#pragma once

namespace CE::RHI
{
	class FrameGraph;
	struct FrameGraphCompileRequest;

	struct FrameGraphExecuteRequest
	{
		FrameGraph* frameGraph = nullptr;

		u32 currentImageIndex = 0;
	};
    
	class CORERHI_API FrameGraphExecuter
	{
	public:
		virtual ~FrameGraphExecuter();

		bool Execute(const FrameGraphExecuteRequest& executeRequest);

		bool Compile(const FrameGraphCompileRequest& compileRequest);

	protected:
		FrameGraphExecuter() = default;

	private:

		virtual bool CompileInternal(const FrameGraphCompileRequest& compileRequest) = 0;

		virtual bool ExecuteInternal(const FrameGraphExecuteRequest& executeRequest) = 0;

	};

} // namespace CE::RHI
