#pragma once

namespace CE::RHI
{

	class CORERHI_API FrameGraphBuilder final
	{
	public:
		FrameGraphBuilder() = default;

		void Begin(FrameGraph* frameGraph);

		inline FrameAttachmentDatabase& GetFrameAttachmentDatabase()
		{
			return frameGraph->attachmentDatabase;
		}

		void BeginScope(const ScopeID& id);

		bool UseAttachment(const ImageScopeAttachmentDescriptor& descriptor,
			ScopeAttachmentUsage usage, ScopeAttachmentAccess access);

		bool UseAttachment(const BufferScopeAttachmentDescriptor& descriptor,
			ScopeAttachmentUsage usage, ScopeAttachmentAccess access);

		bool PresentSwapChain(SwapChain* swapChain);

		Scope* EndScope();

		bool End();

	protected:

		FrameGraph* frameGraph = nullptr;
		Scope* currentScope = nullptr;

	};
    
} // namespace CE::RHI
