#pragma once

namespace CE::RHI
{
	struct FrameSchedulerDescriptor
	{
        
	};

	//! FrameScheduler provides user facing API to construct, compile and execute FrameGraph.
	//! Also manages transient memory allocation and aliasing.
	class CORERHI_API FrameScheduler final : public FrameGraphBuilder
	{
		CE_NO_COPY(FrameScheduler)
	public:

		FrameScheduler(const FrameSchedulerDescriptor& descriptor);
		virtual ~FrameScheduler();
		
		inline FrameAttachmentDatabase& GetAttachmentDatabase() const { return frameGraph->attachmentDatabase; }

		inline FrameGraph* GetFrameGraph() const { return frameGraph; }

		void BeginFrameGraph();
        
		//! @brief Call it after EndFrameGraph()
		void Construct();

		//! @brief Compile the transient attachments, and everything.
		void Compile();

		void BeginFrame();

		void EndFrame();
        
        FrameAttachment* GetFrameAttachment(AttachmentID id);

	private:
        
        TransientMemoryPool* transientMemoryPool = nullptr;

		ResourceMemoryRequirements bufferReq{};
		ResourceMemoryRequirements imageReq{};
	};
    
} // namespace CE::RHI
