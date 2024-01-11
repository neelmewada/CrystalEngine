#pragma once

namespace CE::RHI
{
	struct FrameGraphExecuteRequest
	{

	};
    
	class CORERHI_API FrameGraphExecuter
	{
	public:
		virtual ~FrameGraphExecuter();

		bool Execute(const FrameGraphExecuteRequest& executeRequest);

	protected:
		FrameGraphExecuter() = default;

	private:

		virtual bool ExecuteInternal(const FrameGraphExecuteRequest& executeRequest) = 0;

	};

} // namespace CE::RHI
