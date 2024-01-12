#pragma once

namespace CE::RHI
{
	class FrameGraph;

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

	protected:
		FrameGraphExecuter() = default;

	private:

		virtual bool ExecuteInternal(const FrameGraphExecuteRequest& executeRequest) = 0;

	};

} // namespace CE::RHI
