#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CScrollArea::CScrollArea()
	{
		allowVerticalScroll = true;
		receiveMouseEvents = true;
		receiveDragEvents = true;

		clipChildren = true;
	}

	CScrollArea::~CScrollArea()
	{
		
	}

	void CScrollArea::SetAllowVerticalScroll(bool allow)
	{
		if (allowVerticalScroll == allow)
			return;

		allowVerticalScroll = allow;
		SetNeedsStyle();
		SetNeedsLayout();
		SetNeedsPaint();
	}

	void CScrollArea::SetAllowHorizontalScroll(bool allow)
	{
		if (allowHorizontalScroll == allow)
			return;

		allowHorizontalScroll = allow;
		SetNeedsStyle();
		SetNeedsLayout();
		SetNeedsPaint();
	}

	Rect CScrollArea::GetVerticalScrollBarRect()
	{
		if (allowVerticalScroll)
		{
			auto app = CApplication::Get();

			Vec2 originalSize = GetComputedLayoutSize();
			f32 originalHeight = originalSize.height;
			f32 contentMaxY = contentSize.height;

			if (contentMaxY > originalHeight + app->styleConstants.scrollSizeBuffer)
			{
				Rect scrollRegion = Rect::FromSize(Vec2(originalSize.width - app->styleConstants.scrollRectWidth, 0),
					Vec2(app->styleConstants.scrollRectWidth, originalHeight));
				f32 scrollRectHeightRatio = originalHeight / contentMaxY;

				Rect scrollRect = Rect::FromSize(scrollRegion.min,
					Vec2(scrollRegion.GetSize().width, Math::Max(scrollRegion.GetSize().height * scrollRectHeightRatio,
						app->styleConstants.minScrollRectSize)));
				scrollRect = scrollRect.Translate(Vec2(0, (originalHeight - scrollRect.GetSize().height) * normalizedScroll.y));

				return scrollRect;
			}
		}

		return Rect();
	}

	void CScrollArea::OnPaint(CPaintEvent* paintEvent)
	{
		Super::OnPaint(paintEvent);


	}

	void CScrollArea::OnPaintOverlay(CPaintEvent* paintEvent)
	{
		Super::OnPaintOverlay(paintEvent);

		CPainter* painter = paintEvent->painter;

		auto app = CApplication::Get();

		CStyle scrollStyle = styleSheet->SelectStyle(this, CStateFlag::Default, CSubControl::ScrollBar);
		CStyle scrollActiveStyle = styleSheet->SelectStyle(this, CStateFlag::Active | CStateFlag::Hovered | CStateFlag::Pressed, CSubControl::ScrollBar);

		if (allowVerticalScroll) // Draw Vertical Scroll Bar
		{
			Vec2 originalSize = GetComputedLayoutSize();
			f32 originalHeight = originalSize.height;
			f32 contentMaxY = contentSize.height;

			if (contentMaxY > originalHeight + app->styleConstants.scrollSizeBuffer)
			{
				Rect scrollRect = GetVerticalScrollBarRect();

				CPen pen{};
				CBrush brush = CBrush(scrollStyle.GetBackgroundColor());

				if (isVerticalScrollHovered || isVerticalScrollPressed)
				{
					brush.SetColor(scrollActiveStyle.GetBackgroundColor());
				}

				painter->SetPen(pen);
				painter->SetBrush(brush);

				painter->DrawRoundedRect(scrollRect, Vec4(1, 1, 1, 1) * app->styleConstants.scrollRectWidth * 0.5f);
			}
			else
			{
				normalizedScroll = Vec2(0, 0);
			}
		}
	}

	void CScrollArea::HandleEvent(CEvent* event)
	{
		auto app = CApplication::Get();

		if (event->IsMouseEvent())
		{
			CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);
			Vec2 globalMousePos = mouseEvent->mousePos;
			Rect screenSpaceWindowRect = GetScreenSpaceRect();
			Vec2 windowSpaceMousePos = globalMousePos - screenSpaceWindowRect.min;
			Vec2 mouseDelta = mouseEvent->mousePos - mouseEvent->prevMousePos;

			if (mouseEvent->type == CEventType::MouseMove && (allowVerticalScroll || allowHorizontalScroll))
			{
				isVerticalScrollHovered = false;
				SetNeedsPaint();

				if (allowVerticalScroll)
				{
					Vec2 originalSize = GetComputedLayoutSize();
					f32 originalHeight = originalSize.height;
					f32 contentMaxY = contentSize.height;

					if (contentMaxY > originalHeight + app->styleConstants.scrollSizeBuffer)
					{
						Rect scrollRect = GetVerticalScrollBarRect();
						scrollRect = LocalToScreenSpaceRect(scrollRect);

						if (scrollRect.Contains(globalMousePos))
						{
							isVerticalScrollHovered = true;
						}
					}
				}
			}
			else if (mouseEvent->type == CEventType::MouseLeave && (allowVerticalScroll || allowHorizontalScroll))
			{
				isVerticalScrollHovered = false;
				SetNeedsPaint();
			}
			else if (mouseEvent->type == CEventType::DragBegin && (allowVerticalScroll || allowHorizontalScroll))
			{
				CDragEvent* dragEvent = (CDragEvent*)event;

				if (allowVerticalScroll)
				{
					Vec2 originalSize = GetComputedLayoutSize();
					f32 originalHeight = originalSize.height;
					f32 contentMaxY = contentSize.height;

					if (contentMaxY > originalHeight + app->styleConstants.scrollSizeBuffer)
					{
						Rect scrollRect = GetVerticalScrollBarRect();
						scrollRect = LocalToScreenSpaceRect(scrollRect);

						if (scrollRect.Contains(globalMousePos))
						{
							dragEvent->draggedWidget = this;
							dragEvent->ConsumeAndStopPropagation(this);
							isVerticalScrollPressed = true;
						}
					}
				}
			}
			else if (mouseEvent->type == CEventType::DragMove && isVerticalScrollPressed)
			{
				CDragEvent* dragEvent = (CDragEvent*)event;

				if (allowVerticalScroll && isVerticalScrollPressed)
				{
					Vec2 originalSize = GetComputedLayoutSize();
					f32 originalHeight = originalSize.height;

					normalizedScroll.y += mouseDelta.y / (originalHeight - GetVerticalScrollBarRect().GetSize().height);
					normalizedScroll.y = Math::Clamp01(normalizedScroll.y);

					dragEvent->ConsumeAndStopPropagation(this);
					SetNeedsLayout();
					SetNeedsPaint();
				}
			}
			else if (mouseEvent->type == CEventType::DragEnd && isVerticalScrollPressed)
			{
				CDragEvent* dragEvent = (CDragEvent*)event;

				isVerticalScrollPressed = false;
				dragEvent->ConsumeAndStopPropagation(this);
			}
			else if (mouseEvent->type == CEventType::MouseWheel && allowVerticalScroll)
			{
				Vec2 originalSize = GetComputedLayoutSize();
				f32 originalHeight = originalSize.height;
				f32 contentMaxY = contentSize.height;

				if (contentMaxY > originalHeight + app->styleConstants.scrollSizeBuffer) // If scrolling is possible
				{
					normalizedScroll.y += -mouseEvent->wheelDelta.y * scrollSensitivity / (contentMaxY - originalHeight);
					normalizedScroll.y = Math::Clamp01(normalizedScroll.y);

					SetNeedsLayout();
					SetNeedsPaint();
				}
			}
		}

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
