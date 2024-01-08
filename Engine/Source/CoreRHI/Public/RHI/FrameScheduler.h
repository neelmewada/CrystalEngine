#pragma once

namespace CE::RHI
{
	struct FrameSchedulerDescriptor
	{

	};

	//! FrameScheduler provides user facing API to construct, compile and execute FrameGraph.
	//! Also manages transient memory allocation and aliasing.
	class CORERHI_API FrameScheduler final
	{
		CE_NO_COPY(FrameScheduler)
	public:

		FrameScheduler(const FrameSchedulerDescriptor& descriptor);
		virtual ~FrameScheduler();
		
		inline FrameAttachmentDatabase& GetAttachmentDatabase() const { return frameGraph->attachmentDatabase; }

		inline FrameGraph* GetFrameGraph() const { return frameGraph; }

		void BeginFrame();

		void EndFrame();

		inline FrameGraphBuilder& GetFrameGraphBuilder() { return builder; }

	private:

		UniquePtr<FrameGraph> frameGraph = nullptr;
		
		FrameGraphBuilder builder{};
	};
    
} // namespace CE::RHI
