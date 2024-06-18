#include "FusionCore.h"

namespace CE
{

	FFusionContext::FFusionContext()
	{

	}

	FFusionContext::~FFusionContext()
	{

	}

	bool FFusionContext::IsNativeContext() const
	{
		return IsOfType<FNativeContext>();
	}

	void FFusionContext::Tick()
	{
		ZoneScoped;

		DoLayout();

		for (FFusionContext* childContext : childContexts)
		{
			childContext->Tick();
		}

		DoPaint();
	}

	void FFusionContext::DoLayout()
	{
		ZoneScoped;

		if (owningWidget && layoutDirty)
		{
			owningWidget->PrecomputeIntrinsicSize();

			owningWidget->computedPosition = Vec2();
			owningWidget->computedSize = availableSize;
			
			owningWidget->PlaceSubWidgets();

			layoutDirty = false;
			dirty = true;
		}
	}

	void FFusionContext::DoPaint()
	{

	}

	void FFusionContext::SetOwningWidget(FWidget* widget)
	{
		this->owningWidget = widget;

		this->owningWidget->SetContextRecursively(this);
	}

	void FFusionContext::OnWidgetDestroyed(FWidget* widget)
	{
		if (owningWidget == widget)
		{
			owningWidget = nullptr;
		}
	}

	void FFusionContext::MarkLayoutDirty()
	{
		layoutDirty = true;
		dirty = true;

		for (FFusionContext* childContext : childContexts)
		{
			childContext->MarkLayoutDirty();
		}
	}

	void FFusionContext::MarkDirty()
	{
		dirty = true;

		for (FFusionContext* childContext : childContexts)
		{
			childContext->MarkDirty();
		}
	}

	void FFusionContext::QueueDestroy()
	{
		if (isDestroyed)
			return;

		FusionApplication::Get()->QueueDestroy(this);
		isDestroyed = true;
	}

	void FFusionContext::AddChildContext(FFusionContext* context)
	{
		if (childContexts.Exists(context))
			return;

		context->parentContext = this;
		childContexts.Add(context);

		FusionApplication::Get()->RebuildFrameGraph();
	}

	void FFusionContext::RemoveChildContext(FFusionContext* context)
	{
		if (!childContexts.Exists(context))
			return;

		context->parentContext = nullptr;
		childContexts.Remove(context);

		FusionApplication::Get()->RebuildFrameGraph();
	}

	void FFusionContext::SetStyleManager(FStyleManager* styleManager)
	{
		this->styleManager = styleManager;
		MarkDirty();
	}

	FStyleManager* FFusionContext::GetStyleManager()
	{
		if (styleManager == nullptr && parentContext)
		{
			return parentContext->GetStyleManager();
		}
		return styleManager;
	}

	void FFusionContext::SetClearScreen(bool set)
	{
		if (clearScreen != set)
		{
			clearScreen = set;

			FusionApplication::Get()->RebuildFrameGraph();
		}
	}

	void FFusionContext::EmplaceFrameAttachments()
	{
		for (FFusionContext* childContext : childContexts)
		{
			childContext->EmplaceFrameAttachments();
		}
	}

	void FFusionContext::EnqueueScopes()
	{
		for (FFusionContext* childContext : childContexts)
		{
			childContext->EnqueueScopes();
		}
	}

	void FFusionContext::SetDrawListMask(RHI::DrawListMask& outMask)
	{
		for (FFusionContext* childContext : childContexts)
		{
			childContext->SetDrawListMask(outMask);
		}
	}

	void FFusionContext::EnqueueDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex)
	{
		for (FFusionContext* childContext : childContexts)
		{
			childContext->EnqueueDrawPackets(drawList, imageIndex);
		}
	}

	void FFusionContext::SetDrawPackets(RHI::DrawListContext& drawList)
	{
		for (FFusionContext* childContext : childContexts)
		{
			childContext->SetDrawPackets(drawList);
		}
	}

} // namespace CE
