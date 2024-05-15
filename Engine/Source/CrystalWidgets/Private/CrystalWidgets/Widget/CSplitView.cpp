#include "CrystalWidgets.h"

namespace CE::Widgets
{
	constexpr f32 splitterWidth = 2.0f;

	CSplitView::CSplitView()
	{
		receiveDragEvents = true;
	}

	CSplitView::~CSplitView()
	{
		
	}

	bool CSplitView::IsSubWidgetAllowed(Class* subWidgetClass)
	{
		return subWidgetClass != nullptr && subWidgetClass->IsSubclassOf<CSplitViewContainer>();
	}

	void CSplitView::SetOrientation(COrientation orientation)
	{
		this->orientation = orientation;

		SetNeedsStyle();
		SetNeedsLayout();
		SetNeedsPaint();
	}

	bool CSplitView::IsLayoutCalculationRoot()
	{
		return true;
	}

	void CSplitView::AddSplit(f32 ratio)
	{
		if (containers.IsEmpty())
		{
			CSplitViewContainer* containerA = CreateObject<CSplitViewContainer>(this, "SplitContainer");
			CSplitViewContainer* containerB = CreateObject<CSplitViewContainer>(this, "SplitContainer");

			containerA->splitRatio = 1.0f - ratio;
			containerB->splitRatio = ratio;

			UpdateSplits();
			return;
		}

		CSplitViewContainer* last = containers.Top();
		f32 totalRatio = last->splitRatio;
		last->splitRatio = (1.0f - ratio) * totalRatio;

		CSplitViewContainer* next = CreateObject<CSplitViewContainer>(this, "SplitContainer");
		next->splitRatio = ratio * totalRatio;
	}

	Vec2 CSplitView::GetAvailableSizeForChild(CWidget* childWidget)
	{
		Vec2 size = GetComputedLayoutSize();
		if (childWidget->IsOfType<CSplitViewContainer>() && containers.Exists((CSplitViewContainer*)childWidget))
		{
			auto child = (CSplitViewContainer*)childWidget;
			if (orientation == COrientation::Horizontal)
				size.x *= child->splitRatio;
			else if (orientation == COrientation::Vertical)
				size.y *= child->splitRatio;
		}
		return size;
	}

	int CSplitView::GetIndexOfContainer(CSplitViewContainer* container) const
	{
		return containers.IndexOf(container);
	}

	void CSplitView::SetContainerSplitRatio(CSplitViewContainer* container, f32 newSplitRatio)
	{
		newSplitRatio = Math::Clamp(newSplitRatio, 0.1f, 0.9f);
		f32 exclusiveRatioSum = 0;
		int totalContainers = containers.GetSize();
		bool found = false;

		for (CSplitViewContainer* child : containers)
		{
			if (child != container)
			{
				exclusiveRatioSum += child->splitRatio;
			}
			else
			{
				found = true;
			}
		}

		if (!found)
			return;

		container->splitRatio = newSplitRatio;

		Array<f32> weights{};
		weights.Reserve(containers.GetSize());

		if (abs((exclusiveRatioSum + newSplitRatio) - 1.0f) > 0.0001f)
		{
			f32 totalSplitRatio = 0.0f;
			for (CSplitViewContainer* child : containers)
			{
				weights.Add(child->splitRatio);
				totalSplitRatio += child->splitRatio;
			}

			for (int i = 0; i < containers.GetSize(); ++i)
			{
				containers[i]->splitRatio = weights[i] / totalSplitRatio;
			}
		}
	}

	void CSplitView::OnBeforeComputeStyle()
	{
		Super::OnBeforeComputeStyle();

		CStyle splitterActiveStyle = styleSheet->SelectStyle(this, CStateFlag::Active, CSubControl::Splitter);
		CStyle splitterStyle = styleSheet->SelectStyle(this, CStateFlag::Default, CSubControl::Splitter);

		splitterColor = splitterStyle.GetBackgroundColor();
		splitterActiveColor = splitterActiveStyle.GetBackgroundColor();
	}

	void CSplitView::OnPaint(CPaintEvent* paintEvent)
	{
		CPainter* painter = paintEvent->painter;

		for (int i = 0; i < containers.GetSize() - 1; i++)
		{
			Rect childRect = Rect::FromSize(containers[i]->GetComputedLayoutTopLeft(), containers[i]->GetComputedLayoutSize());

			painter->SetBrush(CBrush((draggedSplitIdx == i || hoveredSplitIdx == i) ? splitterActiveColor : splitterColor));
			painter->SetPen(CPen());

			if (orientation == COrientation::Horizontal)
			{
				Rect splitterRect = Rect::FromSize(childRect.min + Vec2(childRect.GetSize().width, 0),
					Vec2(splitterWidth * 2, childRect.GetSize().height));

				painter->DrawRect(splitterRect);
			}
			else if (orientation == COrientation::Vertical)
			{
				Rect splitterRect = Rect::FromSize(childRect.min + Vec2(0, childRect.GetSize().height),
					Vec2(childRect.GetSize().width, splitterWidth * 2));

				painter->DrawRect(splitterRect);
			}
		}

		Super::OnPaint(paintEvent);
	}

	void CSplitView::HandleEvent(CEvent* event)
	{
		if (event->IsMouseEvent() || event->IsDragEvent())
		{
			CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);

			if (event->type == CEventType::MouseMove && mouseEvent->button == MouseButton::None)
			{
				hoveredSplitIdx = -1;

				for (int i = 0; i < containers.GetSize() - 1; i++)
				{
					Rect childRect = containers[i]->GetScreenSpaceRect();

					if (orientation == COrientation::Horizontal)
					{
						Rect splitterRect = Rect::FromSize(childRect.min + Vec2(childRect.GetSize().width, 0),
							Vec2(splitterWidth * 2, childRect.GetSize().height));

						if (splitterRect.Contains(mouseEvent->mousePos))
						{
							hoveredSplitIdx = i;
							break;
						}
					}
					else if (orientation == COrientation::Vertical)
					{
						Rect splitterRect = Rect::FromSize(childRect.min + Vec2(0, childRect.GetSize().height),
							Vec2(childRect.GetSize().width, splitterWidth * 2));

						if (splitterRect.Contains(mouseEvent->mousePos))
						{
							hoveredSplitIdx = i;
							break;
						}
					}
				}

				if (hoveredSplitIdx >= 0)
				{
					if (hasPushedResizeCursor)
					{
						if ((orientation == COrientation::Horizontal && CApplication::Get()->GetTopCursor() != CCursor::SizeH) ||
							(orientation == COrientation::Vertical && CApplication::Get()->GetTopCursor() != CCursor::SizeV))
						{
							CApplication::Get()->PopCursor();
							hasPushedResizeCursor = false;
						}
					}
					if (!hasPushedResizeCursor)
					{
						CApplication::Get()->PushCursor(orientation == COrientation::Horizontal ? CCursor::SizeH : CCursor::SizeV);
						hasPushedResizeCursor = true;
					}
				}
				else
				{
					if (hasPushedResizeCursor)
					{
						CApplication::Get()->PopCursor();
						hasPushedResizeCursor = false;
					}
				}

				SetNeedsPaint();
			}
			else if (event->type == CEventType::MouseLeave)
			{
				if (hasPushedResizeCursor && draggedSplitIdx < 0)
				{
					hoveredSplitIdx = -1;

					CApplication::Get()->PopCursor();
					hasPushedResizeCursor = false;

					SetNeedsPaint();
				}
			}

			if (!event->isConsumed && event->IsDragEvent())
			{
				CDragEvent* dragEvent = static_cast<CDragEvent*>(event);

				if (dragEvent->type == CEventType::DragBegin)
				{
					if (hoveredSplitIdx >= 0)
					{
						draggedSplitIdx = hoveredSplitIdx;

						dragEvent->draggedWidget = this;
						dragEvent->ConsumeAndStopPropagation(this);

						SetNeedsPaint();
					}
				}
				else if (dragEvent->type == CEventType::DragMove)
				{
					if (draggedSplitIdx >= 0)
					{
						Vec2 mouseDelta = mouseEvent->mousePos - mouseEvent->prevMousePos;

						if (orientation == COrientation::Vertical)
						{
							mouseDelta.y /= GetComputedLayoutSize().y;
							for (int i = 0; i < containers.GetSize(); i++)
							{
								if (i == draggedSplitIdx && i < containers.GetSize() - 1)
								{
									containers[i]->splitRatio += mouseDelta.y;
									containers[i]->splitRatio = Math::Clamp(containers[i]->splitRatio, 0.1f, 0.9f);
									containers[i + 1]->splitRatio -= mouseDelta.y;
									containers[i + 1]->splitRatio = Math::Clamp(containers[i + 1]->splitRatio, 0.1f, 0.9f);

									emit OnSplitterDragged(this, draggedSplitIdx);

									SetNeedsStyle();
									SetNeedsLayout();
									SetNeedsPaint();
									break;
								}
							}

							dragEvent->draggedWidget = this;
							dragEvent->ConsumeAndStopPropagation(this);
						}
						else if (orientation == COrientation::Horizontal)
						{
							mouseDelta.x /= GetComputedLayoutSize().x;
							for (int i = 0; i < containers.GetSize(); i++)
							{
								if (i == draggedSplitIdx && i < containers.GetSize() - 1)
								{
									containers[i]->splitRatio += mouseDelta.x;
									containers[i]->splitRatio = Math::Clamp(containers[i]->splitRatio, 0.1f, 0.9f);
									containers[i + 1]->splitRatio -= mouseDelta.x;
									containers[i + 1]->splitRatio = Math::Clamp(containers[i + 1]->splitRatio, 0.1f, 0.9f);

									emit OnSplitterDragged(this, draggedSplitIdx);

									SetNeedsStyle();
									SetNeedsLayout();
									SetNeedsPaint();
									break;
								}
							}

							dragEvent->draggedWidget = this;
							dragEvent->ConsumeAndStopPropagation(this);
						}
					}
				}
				else if (dragEvent->type == CEventType::DragEnd)
				{
					if (draggedSplitIdx >= 0)
					{
						draggedSplitIdx = -1;
					}

					if (hasPushedResizeCursor)
					{
						CApplication::Get()->PopCursor();
						hasPushedResizeCursor = false;
					}
				}
			}
		}

		Super::HandleEvent(event);
	}

	void CSplitView::OnSubobjectAttached(Object* subobject)
	{
		Super::OnSubobjectAttached(subobject);

		if (subobject->IsOfType<CSplitViewContainer>())
		{
			CSplitViewContainer* container = static_cast<CSplitViewContainer*>(subobject);
			container->splitView = this;

			if (containers.GetSize() >= 10) // Only 10 max split views allowed
			{
				RemoveSubWidget(container);
				return;
			}

			containers.Add(container);

			UpdateSplits();
		}
	}

	void CSplitView::OnSubobjectDetached(Object* subobject)
	{
		Super::OnSubobjectDetached(subobject);

		if (subobject->IsOfType<CSplitViewContainer>())
		{
			CSplitViewContainer* container = static_cast<CSplitViewContainer*>(subobject);
			container->splitView = nullptr;

			containers.Remove(container);

			UpdateSplits();
		}
	}

	void CSplitView::UpdateSplits()
	{
		f32 remainingRatio = 1.0f;
		f32 maxRatio = 0;
		int maxRatioIndex = -1;

		for (int i = 0; i < containers.GetSize(); i++)
		{
			auto container = containers[i];
			if (container->splitRatio > maxRatio)
			{
				maxRatio = container->splitRatio;
				maxRatioIndex = i;
			}

			remainingRatio -= container->splitRatio;
		}

		if (remainingRatio < 0)
		{
			
		}
	}

} // namespace CE::Widgets
