#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CToolWindow::CToolWindow()
	{
		receiveMouseEvents = true;
		canBeClosed = true;
		canBeMaximized = canBeMinimized = false;

		allowVerticalScroll = allowHorizontalScroll = false;
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

		if (nativeWindow)
		{
			Vec2 size = GetComputedLayoutSize() - Vec2(rootPadding.x + rootPadding.z, rootPadding.y + rootPadding.w);
			Vec2 pos = GetComputedLayoutTopLeft() + rootPadding.min;


		}
	}

	void CToolWindow::HandleEvent(CEvent* event)
	{


		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
