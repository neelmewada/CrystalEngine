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

		if (object->IsOfType<CWidget>() && !IsOfType<CWindow>())
		{
			((CWidget*)object)->ownerWindow = ownerWindow;
		}
	}

	void CWidget::OnSubobjectDetached(Object* object)
	{
		Super::OnSubobjectDetached(object);

		if (!object)
			return;

		if (object->IsOfType<CWidget>() && !IsOfType<CWindow>())
		{
			((CWidget*)object)->ownerWindow = nullptr;
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
		// TODO: Temporary code only for testing
		CPainter* painter = paintEvent->painter;

		CPen pen;
		pen.SetColor(Color(1, 1, 0, 1));
		pen.SetWidth(5.0);
		painter->SetPen(pen);

		CBrush brush;
		brush.SetColor(Color(1, 1, 1, 1));
		
		painter->SetBrush(brush);

		painter->DrawRoundedRect(Rect(50, 50, 200, 100), Vec4(5, 10, 15, 20));

		CFont font{};
		font.SetFamily("Poppins");
		font.SetSize(14);
		font.SetBold(false);

		painter->SetFont(font);
		painter->DrawText("This is first sentence. This is second sentence.\nThis is third sentence.\nThis is fourth sentence.", Vec2(0, 100));
	}

	void CWidget::HandleEvent(CEvent* event)
	{
		if (event == nullptr)
			return;

		// Handle event for this widget

		if (event->type == CEventType::PaintEvent)
		{
			CPaintEvent* paintEvent = (CPaintEvent*)event;
			if (paintEvent->painter != nullptr)
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
