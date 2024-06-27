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

		DoPaint();

		// TODO: Better integration of child contexts and native popups

		TickInput();

		for (FFusionContext* childContext : childContexts)
		{
			childContext->Tick();
		}
	}

	void FFusionContext::DoLayout()
	{
		ZoneScoped;

		if (owningWidget && layoutDirty)
		{
			owningWidget->CalculateIntrinsicSize();

			owningWidget->computedPosition = Vec2();
			owningWidget->computedSize = availableSize;
			
			owningWidget->PlaceSubWidgets();

			for (FPopup* popup : localPopupStack)
			{
				popup->CalculateIntrinsicSize();

				Vec2 size = popup->initialSize;
				if (size.x <= 0 || size.y <= 0)
				{
					size = popup->intrinsicSize;
				}

				popup->computedPosition = popup->initialPos;
				popup->computedSize = size;

				popup->PlaceSubWidgets();
			}

			layoutDirty = false;
			dirty = true;
		}
	}

	void FFusionContext::DoPaint()
	{

	}

	void FFusionContext::TickInput()
	{

	}

	void FFusionContext::SetOwningWidget(FWidget* widget)
	{
		this->owningWidget = widget;

		this->owningWidget->SetContextRecursively(this);
	}

	FStyleSet* FFusionContext::GetDefaultStyleSet()
	{
		if (!defaultStyleSet && parentContext)
		{
			return parentContext->GetDefaultStyleSet();
		}
		return defaultStyleSet;
	}

	void FFusionContext::OnWidgetDestroyed(FWidget* widget)
	{
		if (owningWidget == widget)
		{
			owningWidget = nullptr;
		}
	}

	bool FFusionContext::IsFocused() const
	{
		if (IsDefaultInstance())
			return false;

		if (parentContext == nullptr) // Root context
			return true;

		for (FFusionContext* childContext : childContexts)
		{
			if (childContext->IsFocused())
				return true;
		}

		return false;
	}

	bool FFusionContext::IsRootContext() const
	{
		return isRootContext;
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

	void FFusionContext::PushLocalPopup(FPopup* popup, Vec2 globalPosition, Vec2 size)
	{
		if (!popup)
			return;
		if (localPopupStack.Exists(popup))
			return;

		localPopupStack.Push(popup);
		popup->context = this;
		popup->initialPos = globalPosition;
		popup->initialSize = size;
		popup->isShown = true;
		popup->isNativePopup = false;
		
		SetFocusWidget(popup);
		MarkLayoutDirty();
	}

	void FFusionContext::PushNativePopup(FPopup* popup, Vec2 globalPosition, Vec2 size)
	{
		
	}

	bool FFusionContext::ClosePopup(FPopup* popup)
	{
		if (!popup)
			return true;

		int index = localPopupStack.IndexOf(popup);
		if (index < 0)
			return false;

		popup->isShown = false;
		popup->context = nullptr;
		localPopupStack.RemoveAt(index);

		if (curFocusWidget != nullptr && curFocusWidget->ParentExistsRecursive(popup))
		{
			if (localPopupStack.NonEmpty())
			{
				SetFocusWidget(localPopupStack.Top());
			}
			else if (owningWidget)
			{
				SetFocusWidget(owningWidget);
			}
		}

		MarkLayoutDirty();

		return true;
	}

	void FFusionContext::SetDefaultStyleSet(FStyleSet* styleSet)
	{
		defaultStyleSet = styleSet;
		MarkLayoutDirty();
	}

	void FFusionContext::SetClearScreen(bool set)
	{
		if (clearScreen != set)
		{
			clearScreen = set;

			FusionApplication::Get()->RebuildFrameGraph();
		}
	}

	void FFusionContext::SetClearColor(const Color& color)
	{
		clearColor = color;
	}

	FPainter* FFusionContext::GetPainter()
	{
		return nullptr;
	}

	void FFusionContext::SetFocusWidget(FWidget* focusWidget)
	{
		widgetToFocus = focusWidget;
	}

	FWidget* FFusionContext::HitTest(Vec2 mousePosition)
	{
		FWidget* hoveredWidget;

		Rect windowRect = Rect::FromSize(Vec2(), GetAvailableSize());
		if (!windowRect.Contains(mousePosition))
			return nullptr;

		for (int i = localPopupStack.GetSize() - 1; i >= 0; --i)
		{
			hoveredWidget = localPopupStack[i]->HitTest(mousePosition);
			if (hoveredWidget || localPopupStack[i]->m_BlockInteraction)
			{
				return hoveredWidget;
			}
		}

		if (!owningWidget)
			return nullptr;

		hoveredWidget = owningWidget->HitTest(mousePosition);
		return hoveredWidget;
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

	void FFusionContext::OnStyleSetDeregistered(FStyleSet* styleSet)
	{
		if (owningWidget != nullptr)
		{
			owningWidget->ClearStyle();
		}

		for (FFusionContext* childContext : childContexts)
		{
			childContext->OnStyleSetDeregistered(styleSet);
		}
	}

} // namespace CE
