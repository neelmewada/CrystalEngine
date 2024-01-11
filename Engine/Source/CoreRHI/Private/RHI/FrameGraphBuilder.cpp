#include "CoreRHI.h"

namespace CE::RHI
{

    void FrameGraphBuilder::BeginFrameGraph(FrameGraph* frameGraph)
    {
		this->frameGraph = frameGraph;
		frameGraph->Clear();
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

	bool FrameGraphBuilder::PresentSwapChain(SwapChain* swapChain)
	{
		if (!currentScope || !frameGraph)
			return false;
		frameGraph->presentSwapChain = swapChain;
		currentScope->presentsSwapChain = true;
		return true;
	}

	Scope* FrameGraphBuilder::EndScope()
	{
		if (!currentScope || !frameGraph)
			return nullptr;
		frameGraph->scopes.Add(currentScope);
		frameGraph->scopesById[currentScope->id] = currentScope;
		return currentScope;
	}

    bool FrameGraphBuilder::EndFrameGraph()
    {
		return frameGraph->Build();
    }

} // namespace CE::RHI
