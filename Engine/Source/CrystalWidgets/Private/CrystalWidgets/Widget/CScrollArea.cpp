#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CScrollArea::CScrollArea()
	{
		allowVerticalScroll = true;
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


		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
