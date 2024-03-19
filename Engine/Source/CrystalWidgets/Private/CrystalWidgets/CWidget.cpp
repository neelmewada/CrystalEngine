#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CWidget::CWidget()
	{
		node = YGNodeNew();
		YGNodeSetContext(node, this);
	}

	CWidget::~CWidget()
	{
		YGNodeFree(node);
		node = nullptr;
	}

	void CWidget::OnAfterConstruct()
	{
		if (!IsDefaultInstance())
		{
			Construct();
		}
	}

	void CWidget::OnSubobjectAttached(Object* object)
	{
		Super::OnSubobjectAttached(object);

		if (!object)
			return;

		if (object->IsOfType<CWidget>())
		{
			CWidget* widget = (CWidget*)object;
			widget->ownerWindow = ownerWindow;
		}
	}

	void CWidget::OnSubobjectDetached(Object* object)
	{
		Super::OnSubobjectDetached(object);

		if (!object)
			return;

		if (object->IsOfType<CWidget>())
		{
			CWidget* widget = (CWidget*)object;
			widget->ownerWindow = nullptr;
		}
	}

	bool CWidget::NeedsPaint()
	{
		if (needsPaint)
			return true;

		for (auto widget : attachedWidgets)
		{
			if (widget != this && widget != nullptr && widget->NeedsPaint())
				return true;
		}

		return false;
	}

	void CWidget::AddSubWidget(CWidget* widget)
	{
		AttachSubobject(widget);

		attachedWidgets.Add(widget);
		widget->parent = this;

		SetNeedsPaint();
	}

	void CWidget::RemoveSubWidget(CWidget* widget)
	{
		DetachSubobject(widget);

		attachedWidgets.Remove(widget);
		widget->parent = nullptr;

		SetNeedsPaint();
	}

	void CWidget::SetNeedsPaintRecursively(bool newValue)
	{
		needsPaint = newValue;

		for (auto widget : attachedWidgets)
		{
			if (widget != this && widget != nullptr)
				widget->SetNeedsPaintRecursively(newValue);
		}
	}

	void CWidget::Construct()
	{
		
	}

	void CWidget::OnPaint(CPaintEvent* paintEvent)
	{
		
	}

	void CWidget::HandleEvent(CEvent* event)
	{
		if (event == nullptr)
			return;

		// Handle event for this widget
		if (event->type == CEventType::PaintEvent)
		{
			CPaintEvent* paintEvent = (CPaintEvent*)event;
			if (paintEvent->painter != nullptr && CanPaint())
			{
				paintEvent->painter->Reset();
				OnPaint(paintEvent);
			}
		}
		
		// Pass event down the chain
		if (event->direction == CEventDirection::TopToBottom)
		{
			for (CWidget* widget : attachedWidgets)
			{
				if (event->isConsumed && event->stopPropagation)
					return;
				widget->HandleEvent(event);
			}
		}
		else if (event->direction == CEventDirection::BottomToTop) // Pass event up the chain
		{
			for (CWidget* parent = this->parent; parent != nullptr; parent = parent->parent)
			{
				if (parent != this) // Prevent infinite recursion!
				{
					parent->HandleEvent(event);
				}
			}
		}
	}
    
} // namespace CE::Widgets
