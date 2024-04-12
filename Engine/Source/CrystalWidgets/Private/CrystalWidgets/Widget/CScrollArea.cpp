#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CScrollArea::CScrollArea()
	{
		allowVerticalScroll = true;
		allowHorizontalScroll = false;

		receiveMouseEvents = true;
		receiveDragEvents = true;

		clipChildren = true;

		AddDefaultBehavior<CScrollBehavior>();
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

	void CScrollArea::OnPaint(CPaintEvent* paintEvent)
	{
		Super::OnPaint(paintEvent);


	}

	void CScrollArea::OnPaintOverlay(CPaintEvent* paintEvent)
	{
		Super::OnPaintOverlay(paintEvent);

		CPainter* painter = paintEvent->painter;

	}

	void CScrollArea::HandleEvent(CEvent* event)
	{
		auto app = CApplication::Get();


		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
