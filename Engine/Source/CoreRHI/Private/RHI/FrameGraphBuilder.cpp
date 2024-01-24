#include "CoreRHI.h"

namespace CE::RHI
{

    void FrameGraphBuilder::BeginFrameGraph(FrameGraph* frameGraph)
    {
		this->frameGraph = frameGraph;
		frameGraph->Clear();

		errorMessages.Clear();
		success = true;
    }

	void FrameGraphBuilder::BeginScopeGroup(const ScopeID& groupId)
	{
		curScopeGroup = {};
		curScopeGroup.groupId = groupId;
	}

	void FrameGraphBuilder::BeginScope(const ScopeID& id)
	{
		RHI::ScopeDescriptor desc{};
		desc.id = id;
		desc.queueClass = RHI::HardwareQueueClass::Graphics;
		this->currentScope = RHI::gDynamicRHI->CreateScope(desc);
	}

	bool FrameGraphBuilder::ScopeQueueClass(HardwareQueueClass queueClass)
	{
		if (!currentScope || !frameGraph)
			return false;
		currentScope->queueClass = queueClass;
		return true;
	}

	bool FrameGraphBuilder::UseAttachment(const ImageScopeAttachmentDescriptor& descriptor, ScopeAttachmentUsage usage, ScopeAttachmentAccess access)
	{
		if (!currentScope || !frameGraph)
			return false;

		FrameAttachment* frameAttachment = GetFrameAttachmentDatabase().FindFrameAttachment(descriptor.attachmentId);
		if (!frameAttachment)
			return false;

		ScopeAttachment* scopeAttachment = currentScope->FindScopeAttachment(descriptor.attachmentId);
		if (scopeAttachment == nullptr)
		{
			scopeAttachment = currentScope->EmplaceScopeAttachment<ImageScopeAttachment>(frameAttachment, usage, access, descriptor);
		}
		else
		{
			scopeAttachment->usage = usage;
			scopeAttachment->access = access;
		}
		
		return true;
	}

	bool FrameGraphBuilder::UseAttachment(const BufferScopeAttachmentDescriptor& descriptor, ScopeAttachmentUsage usage, ScopeAttachmentAccess access)
	{
		if (!currentScope || !frameGraph)
			return false;

		FrameAttachment* frameAttachment = GetFrameAttachmentDatabase().FindFrameAttachment(descriptor.attachmentId);
		if (!frameAttachment)
			return false;

		ScopeAttachment* scopeAttachment = currentScope->FindScopeAttachment(descriptor.attachmentId);
		if (scopeAttachment == nullptr)
		{
			scopeAttachment = currentScope->EmplaceScopeAttachment<BufferScopeAttachment>(frameAttachment, usage, access, descriptor);
		}
		else
		{
			scopeAttachment->usage = usage;
			scopeAttachment->access = access;
		}

		return true;
	}

	bool FrameGraphBuilder::UsePipelines(const Array<RHI::PipelineState*>& pipelines)
	{
		if (!currentScope || !frameGraph)
			return false;

		currentScope->usePipelines.AddRange(pipelines);

		return true;
	}

	bool FrameGraphBuilder::UsePipeline(RHI::PipelineState* pipeline)
	{
		if (!currentScope || !frameGraph || !pipeline)
			return false;

		currentScope->usePipelines.Add(pipeline);

		return true;
	}

	bool FrameGraphBuilder::PresentSwapChain(SwapChain* swapChain)
	{
		if (!currentScope || !frameGraph)
			return false;
		frameGraph->numFramesInFlight = swapChain->GetImageCount();
		frameGraph->presentSwapChain = swapChain;
		frameGraph->presentingScope = currentScope;
		currentScope->presentsSwapChain = true;
		return true;
	}

	Scope* FrameGraphBuilder::EndScope()
	{
		if (!currentScope || !frameGraph)
			return nullptr;
		if (curScopeGroup.groupId.IsValid())
			curScopeGroup.scopes.Add(currentScope);

		frameGraph->scopes.Add(currentScope);
		frameGraph->scopesById[currentScope->id] = currentScope;
		return currentScope;
	}

	void FrameGraphBuilder::EndScopeGroup()
	{
		int scopeGroupIndex = frameGraph->scopeGroups.GetSize();
		frameGraph->scopeGroups.Add(curScopeGroup);
		curScopeGroup = {};

		const Array<Scope*>& scopesList = frameGraph->scopeGroups.GetLast().scopes;

		RHI::HardwareQueueClass queueClass{};

		for (int i = 0; i < scopesList.GetSize(); i++)
		{
			Scope* cur = scopesList[i];
			Scope* prev = nullptr;
			Scope* next = nullptr;
			cur->scopeGroupIndex = scopeGroupIndex;

			if (i == 0)
			{
				queueClass = cur->queueClass;
			}
			else if (queueClass != cur->queueClass)
			{
				errorMessages.Add("All scopes inside scope group must have same HardwareQueueClass.");
				CE_LOG(Error, All, errorMessages.Top());
				success = false;
				return;
			}

			if (i < scopesList.GetSize() - 1)
				next = scopesList[i + 1];
			if (i > 0)
				prev = scopesList[i - 1];

			cur->prevSubPass = prev;
			cur->nextSubPass = next;
		}
	}

    bool FrameGraphBuilder::EndFrameGraph()
    {
		return frameGraph->Build();
    }

} // namespace CE::RHI
