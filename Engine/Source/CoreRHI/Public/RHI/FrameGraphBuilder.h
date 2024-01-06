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

		void BeginScope(const Name& id);

		bool UseAttachment(const ImageScopeAttachmentDescriptor& descriptor,
			ScopeAttachmentUsage usage, ScopeAttachmentAccess access);

		bool UseAttachment(const BufferScopeAttachmentDescriptor& descriptor,
			ScopeAttachmentUsage usage, ScopeAttachmentAccess access);

		Scope* EndScope();

		bool End();

	private:

		FrameGraph* frameGraph = nullptr;
		Scope* currentScope = nullptr;

	};
    
} // namespace CE::RHI
