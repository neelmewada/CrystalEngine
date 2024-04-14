#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CTabWidget::CTabWidget()
	{
		rootPadding = Vec4(0, 30, 0, 0);
	}

	CTabWidget::~CTabWidget()
	{
		
	}

	bool CTabWidget::IsSubWidgetAllowed(Class* subWidgetClass)
	{
		return subWidgetClass->IsSubclassOf<CTabWidgetContainer>();
	}

	void CTabWidget::SetActiveTab(int tabIndex)
	{
		if (tabIndex < 0 || tabIndex >= containers.GetSize() || activeTab == containers[tabIndex])
			return;

		if (activeTab)
			activeTab->SetEnabled(false);

		activeTab = containers[tabIndex];
		activeTab->SetEnabled(true);

		emit OnTabSelectionChanged(tabIndex);

		SetNeedsPaint();
	}

	int CTabWidget::GetActiveTabIndex() const
	{
		for (int i = 0; i < containers.GetSize(); ++i)
		{
			if (activeTab == containers[i])
				return i;
		}
		return -1;
	}

	void CTabWidget::OnAfterComputeStyle()
	{
		Super::OnAfterComputeStyle();

	}

	void CTabWidget::OnPaint(CPaintEvent* paintEvent)
	{
		Super::OnPaint(paintEvent);

		CPainter* painter = paintEvent->painter;

		CStyle tabStyle = styleSheet->SelectStyle(this, CStateFlag::Default, CSubControl::Tab);
		CStyle tabActiveStyle = styleSheet->SelectStyle(this, CStateFlag::Active, CSubControl::Tab);
		CStyle tabHoveredStyle = styleSheet->SelectStyle(this, CStateFlag::Hovered, CSubControl::Tab);

		Vec4 borderRadius = tabStyle.GetBorderRadius();

		CFont font = CFont(tabStyle.GetFontName(), tabStyle.GetFontSize());
		painter->SetFont(font);

		CPen pen = CPen(tabStyle.GetForegroundColor());
		pen.SetWidth(0.0f);
		painter->SetPen(pen);

		Vec4 tabPadding = tabStyle.GetPadding();

		tabRects.Clear();
		tabRects.Reserve(containers.GetSize());
		f32 posX = 0.0f;
		f32 minY = 0.0f;
		f32 maxY = 0.0f;

		for (int i = 0; i < containers.GetSize(); ++i)
		{
			CTabWidgetContainer* container = containers[i];

			const String& title = container->GetTitle();

			Vec2 textSize = painter->CalculateTextSize(title);

			Vec2 totalSize = textSize + Vec2(tabPadding.left + tabPadding.right, tabPadding.top + tabPadding.bottom);

			tabRects.Add(Rect::FromSize(posX, minY, totalSize.width, totalSize.height));

			maxY = Math::Max(maxY, minY + totalSize.height);
			posX += totalSize.width;
		}

		for (int i = 0; i < containers.GetSize(); ++i)
		{
			tabRects[i].max.y = maxY;

			CBrush brush = CBrush(tabStyle.GetBackgroundColor());

			if (containers[i] == activeTab)
			{
				brush.SetColor(tabActiveStyle.GetBackgroundColor());
			}
			else if (hoveredTab == i)
			{
				brush.SetColor(tabHoveredStyle.GetBackgroundColor());
			}

			painter->SetBrush(brush);

			painter->PushChildCoordinateSpace(GetComputedLayoutTopLeft());

			painter->DrawRoundedRect(tabRects[i], borderRadius);
			painter->DrawText(containers[i]->GetTitle(), tabRects[i].min + tabPadding.min);

			painter->PopChildCoordinateSpace();
		}

		Vec4 newRootPadding = Vec4(0, maxY - minY - 1, 0, 0);
		
		if (rootPadding != newRootPadding)
		{
			rootPadding = newRootPadding;
			SetNeedsStyle();
			SetNeedsLayout();
		}
	}

	void CTabWidget::HandleEvent(CEvent* event)
	{
		if (event->IsMouseEvent())
		{
			CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);
			Vec2 globalMousePos = mouseEvent->mousePos;

			if (event->type == CEventType::MouseEnter || event->type == CEventType::MouseMove)
			{
				auto newHoveredTab = -1;

				for (int i = 0; i < tabRects.GetSize(); ++i)
				{
					Rect screenSpaceTabRect = LocalToScreenSpaceRect(tabRects[i]);

					if (screenSpaceTabRect.Contains(globalMousePos))
					{
						newHoveredTab = i;
						break;
					}
				}

				if (hoveredTab != newHoveredTab)
				{
					hoveredTab = newHoveredTab;
					SetNeedsPaint();
				}
			}
			else if (event->type == CEventType::MouseLeave)
			{
				hoveredTab = -1;
				SetNeedsPaint();
			}
			else if (event->type == CEventType::MousePress && !event->isConsumed && mouseEvent->button == MouseButton::Left && hoveredTab >= 0)
			{
				SetActiveTab(hoveredTab);
			}
		}

		Super::HandleEvent(event);
	}


	void CTabWidget::OnSubobjectAttached(Object* object)
	{
		Super::OnSubobjectAttached(object);

		if (!object)
			return;

		if (object->IsOfType<CTabWidgetContainer>())
		{
			CTabWidgetContainer* container = static_cast<CTabWidgetContainer*>(object);
			containers.Add(container);

			if (containers.GetSize() == 1)
			{
				activeTab = container;
				container->SetEnabled(true);
			}
			else
			{
				container->SetEnabled(false);
			}
		}
	}

	void CTabWidget::OnSubobjectDetached(Object* object)
	{
		Super::OnSubobjectDetached(object);

		if (!object)
			return;

		if (object->IsOfType<CTabWidgetContainer>())
		{
			CTabWidgetContainer* container = static_cast<CTabWidgetContainer*>(object);
			containers.Remove(container);

			if (container == activeTab)
			{
				container = containers.NonEmpty() ? containers[0] : nullptr;
				activeTab = container;

				if (container)
					container->SetEnabled(true);
			}
		}
	}

} // namespace CE::Widgets
