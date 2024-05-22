#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CToolWindow::CToolWindow()
	{
		receiveMouseEvents = true;
		canBeClosed = true;
		canBeMaximized = canBeMinimized = false;
		clipChildren = true;

		allowVerticalScroll = allowHorizontalScroll = false;

		rootPadding = Vec4(2, 35, 2, 2);
	}

	CToolWindow::~CToolWindow()
	{
		
	}

	void CToolWindow::Construct()
	{
		Super::Construct();
	}

	void CToolWindow::OnPlatformWindowSet()
	{
		Super::OnPlatformWindowSet();

		if (nativeWindow)
		{
			nativeWindow->GetPlatformWindow()->SetHitTestDelegate(MemberDelegate(&CWindow::WindowHitTest, (CWindow*)this));
		}
	}

	void CToolWindow::OnBeforeDestroy()
	{
		Super::OnBeforeDestroy();
	}

	void CToolWindow::OnPaint(CPaintEvent* paintEvent)
	{
		Super::OnPaint(paintEvent);

	}

	void CToolWindow::OnPaintOverlay(CPaintEvent* paintEvent)
	{
		CPainter* painter = paintEvent->painter;

		if (nativeWindow)
		{
			auto titleBarStyle = styleSheet->SelectStyle(this, CStateFlag::Default, CSubControl::TitleBar);

			Color titleBarColor = titleBarStyle.GetBackgroundColor();
			Color foreground = titleBarStyle.GetForegroundColor();

			CFont font = CFont(titleBarStyle.GetFontName(), titleBarStyle.GetFontSize());
			painter->SetFont(font);

			Vec2 size = GetComputedLayoutSize();
			Vec2 pos = GetComputedLayoutTopLeft();

			if (titleBarColor.a > 0)
			{
				painter->SetBrush(CBrush(titleBarColor));
				painter->SetPen(CPen());

				painter->DrawRect(Rect::FromSize(0, 0, size.width, rootPadding.top));
			}

			if (!title.IsEmpty())
			{
				Vec2 titleSize = painter->CalculateTextSize(title);

				painter->SetPen(CPen(foreground));

				painter->DrawText(title, Rect::FromSize(size.width / 2 - titleSize.width / 2, rootPadding.top / 2 - titleSize.height / 2,
					titleSize.width, titleSize.height));
			}
		}

		Super::OnPaintOverlay(paintEvent);

		if (nativeWindow)
		{
			CPen pen = CPen(Color::RGBA(15, 15, 15));
			pen.SetWidth(2.0f);

			painter->SetBrush(CBrush());
			painter->SetPen(pen);

			u32 w, h;
			nativeWindow->GetWindowSize(&w, &h);

			painter->DrawRect(Rect::FromSize(Vec2(), Vec2(w, h)));

			pen.SetColor(Color::RGBA(50, 50, 50));
			pen.SetWidth(1.0f);
			painter->SetPen(pen);

			painter->DrawRect(Rect::FromSize(Vec2(2, 2), Vec2(w - 4, h - 4)));
		}
	}

	void CToolWindow::HandleEvent(CEvent* event)
	{


		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
