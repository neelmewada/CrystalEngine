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

		for (FFusionContext* childContext : childContexts)
		{
			childContext->DoLayout();
		}
	}

	void FFusionContext::DoPaint()
	{

	}

	void FFusionContext::TickInput()
	{
		ZoneScoped;

	}

	FFusionContext* FFusionContext::GetRootContext() const
	{
		return FusionApplication::Get()->rootContext;
	}

	void FFusionContext::SetOwningWidget(FWidget* widget)
	{
		owningWidget = widget;

		if (owningWidget)
		{
			owningWidget->SetContextRecursively(this);
		}
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

	bool FFusionContext::IsShown() const
	{
		if (parentContext == nullptr)
			return true;

		return true;
	}

	bool FFusionContext::IsRootContext() const
	{
		return IsOfType<FRootContext>();
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

		if (parentContext)
		{
			parentContext->RemoveChildContext(this);
		}

		if (owningWidget)
		{
			owningWidget->SetContextRecursively(nullptr);
		}

		SetOwningWidget(nullptr);

		FusionApplication::Get()->destructionQueue.Add({ this, 1 });
		isDestroyed = true;

		OnQueuedDestroy();
	}

	void FFusionContext::OnQueuedDestroy()
	{
		
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

		GetRootContext()->SetFocusWidget(popup);
		MarkLayoutDirty();
	}

	void FFusionContext::PushNativePopup(FPopup* popup, Vec2 globalPosition, Vec2 size)
	{
		
	}

	bool FFusionContext::ClosePopup(FPopup* popup)
	{
		if (!popup)
			return true;
		if (!popup->isShown)
			return true;

		if (popup->isNativePopup)
		{
			FFusionContext* popupContext = popup->GetContext();
			if (popupContext)
			{
				popupContext->QueueDestroy();
			}

			return true;
		}
		
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

	bool FFusionContext::IsPopupWindow() const
	{
		return owningWidget != nullptr && owningWidget->IsOfType<FPopup>();
	}

	Vec2 FFusionContext::GlobalToScreenSpacePosition(Vec2 pos)
	{
		return pos;
	}

	Vec2 FFusionContext::ScreenToGlobalSpacePosition(Vec2 pos)
	{
		return pos;
	}

	FWidget* FFusionContext::HitTest(Vec2 mousePosition)
	{
		FWidget* hoveredWidget;

		Vec2 screenPos = GlobalToScreenSpacePosition(mousePosition);

		for (int i = childContexts.GetSize() - 1; i >= 0; --i)
		{
			FFusionContext* context = childContexts[i];

			if (context->IsNativeContext())
			{
				FNativeContext* nativeContext = static_cast<FNativeContext*>(context);
				
			}

			hoveredWidget = context->HitTest(context->ScreenToGlobalSpacePosition(screenPos));

			if (context->IsPopupWindow())
			{
				FPopup* popup = static_cast<FPopup*>(context->owningWidget);
				if (popup->BlockInteraction())
				{
					return hoveredWidget;
				}
			}

			if (!context->IsFocused())
				continue;

			if (hoveredWidget)
			{
				return hoveredWidget;
			}
		}

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
