#pragma once

namespace CE::RHI
{
	class FrameGraphCompiler;
	class FrameGraphExecuter;

	struct FrameSchedulerDescriptor
	{
		//! @brief Number of frames being rendered simultaneously (ex: triple buffering = 3).
		//! We don't support more than 1 frames for now.
		const u32 numFramesInFlight = 1;
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

		void Execute();

		void SetScopeDrawList(const ScopeID& scopeId, DrawListContext* drawList);

		RHI::Scope* FindScope(const ScopeID& scopeId);
        
        FrameAttachment* GetFrameAttachment(AttachmentID id) const;

		inline TransientMemoryPool* GetTransientPool() const { return transientMemoryPool; }

	private:

		u32 numFramesInFlight = 1;

		RHI::Scope* drawListScope = nullptr;
        
        TransientMemoryPool* transientMemoryPool = nullptr;

		FrameGraphCompiler* compiler = nullptr;
		FrameGraphExecuter* executer = nullptr;
	};
    
} // namespace CE::RHI
