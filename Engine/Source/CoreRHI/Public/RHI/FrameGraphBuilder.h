#pragma once

namespace CE::RHI
{

	class CORERHI_API FrameGraphBuilder
	{
	public:
		FrameGraphBuilder() = default;

		void BeginFrameGraph(FrameGraph* frameGraph);

		inline FrameAttachmentDatabase& GetFrameAttachmentDatabase()
		{
			return frameGraph->attachmentDatabase;
		}

		void BeginScopeGroup(const ScopeID& groupId);

		void BeginScope(const ScopeID& id);

		bool ScopeQueueClass(HardwareQueueClass queueClass);

		bool UseAttachment(const ImageScopeAttachmentDescriptor& descriptor,
			ScopeAttachmentUsage usage, ScopeAttachmentAccess access);

		bool UseShaderResourceGroup(RHI::ShaderResourceGroup* srg);

		bool UseAttachment(const BufferScopeAttachmentDescriptor& descriptor,
			ScopeAttachmentUsage usage, ScopeAttachmentAccess access);

		bool UsePipelines(const Array<RHI::PipelineState*>& pipelines);
		bool UsePipeline(RHI::PipelineState* pipeline);

		bool ExecuteOnlyIf(const Name& variableName, FrameGraphVariableComparison comparisonOp, const FrameGraphVariable& comparisonValue, bool shouldClear = false);
		 
		bool SetVariableAfterExecution(const Name& variableName, const FrameGraphVariable& value);

		bool SetVariableAfterExecutionForAllFrames(const Name& variableName, const FrameGraphVariable& value);

		bool SetVariableInitialValue(const Name& variableName, const FrameGraphVariable& value);

		bool SetDispatchGroupCount(u32 groupCountX, u32 groupCountY, u32 groupCountZ);

		bool PresentSwapChain(SwapChain* swapChain);

		Scope* EndScope();

		void EndScopeGroup();

		bool EndFrameGraph();

	protected:

		bool success = true;
		Array<String> errorMessages = {};

		FrameGraph* frameGraph = nullptr;
		Scope* currentScope = nullptr;
		ScopeGroup curScopeGroup{};

	};
    
} // namespace CE::RHI
