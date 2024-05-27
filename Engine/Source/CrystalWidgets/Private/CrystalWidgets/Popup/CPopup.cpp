#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CPopup::CPopup()
	{
		allowVerticalScroll = false;
		allowHorizontalScroll = false;

		canBeClosed = canBeMaximized = canBeMinimized = false;
	}

	CPopup::~CPopup()
	{

	}

	void CPopup::OnBeforeDestroy()
	{
		Super::OnBeforeDestroy();
	}

	bool CPopup::IsSubWidgetAllowed(Class* subWidgetClass)
	{
		return subWidgetClass->IsSubclassOf<CWidget>();
	}

	void CPopup::Show()
	{
		SetEnabled(true);

		if (nativeWindow)
		{
			nativeWindow->Show();
			return;
		}

		PlatformWindowInfo windowInfo{};
		windowInfo.windowFlags = PlatformWindowFlags::SkipTaskbar | PlatformWindowFlags::PopupMenu;
		windowInfo.fullscreen = windowInfo.maximised = windowInfo.resizable = false;
		windowInfo.hidden = true;

		CPlatformWindow* parentWindow = GetNativeWindow();

		nativeWindow = new CPlatformWindow(this, showSize.width, showSize.height, windowInfo, parentWindow);

		nativeWindow->GetPlatformWindow()->SetWindowPosition(showPosition);
		nativeWindow->GetPlatformWindow()->SetBorderless(true);
		nativeWindow->GetPlatformWindow()->SetAlwaysOnTop(true);
		nativeWindow->Show();

		nativeWindow->GetPlatformWindow()->SetInputFocus();

		nativeWindow->GetPlatformWindow()->SetHitTestDelegate(MemberDelegate(&Self::WindowHitTest, this));

		SetNeedsLayout();
		SetNeedsStyle();
		SetNeedsPaint();
	}

	void CPopup::Hide()
	{
		if (!nativeWindow)
			return;

		nativeWindow->GetPlatformWindow()->SetHitTestDelegate(nullptr);

		delete nativeWindow;
		nativeWindow = nullptr;

		SetEnabled(false);
	}

	void CPopup::Show(Vec2i screenPosition, Vec2i size)
	{
		showPosition = screenPosition;
		showSize = size;
		windowSize = showSize.ToVec2();

		Show();
	}

	void CPopup::ShowCenteredScreen(Vec2i size)
	{
		Vec2i screenSize = CApplication::Get()->GetScreenBounds(0).GetSize().ToVec2i();

		showPosition = screenSize / 2 - size / 2;
		showSize = size;
		windowSize = showSize.ToVec2();

		Show();
	}

	bool CPopup::IsShown()
	{
		return nativeWindow != nullptr;
	}

	void CPopup::OnFocusLost()
	{
		Super::OnFocusLost();
	}

	bool CPopup::WindowHitTest(PlatformWindow* window, Vec2 position)
	{
		return false;
	}

	void CPopup::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
