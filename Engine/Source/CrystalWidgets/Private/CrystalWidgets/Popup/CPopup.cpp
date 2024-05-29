#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CPopup::CPopup()
	{
		allowVerticalScroll = false;
		allowHorizontalScroll = false;

		canBeClosed = canBeMaximized = canBeMinimized = false;
		enabled = false;

#if PAL_TRAIT_BUILD_EDITOR
		showNativeWindow = true;
#else
		showNativeWindow = false;
#endif

		// TODO: Testing only
		showNativeWindow = false;
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

#if PAL_TRAIT_BUILD_STANDALONE
		showNativeWindow = false;
#endif

		if (showNativeWindow)
		{
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
		else
		{
			rootWindow = GetRootWindow();
			if (!rootWindow)
				return;

			rootWindow->AttachSubWindow(this);

			SetNeedsLayout();
			SetNeedsStyle();
			SetNeedsPaint();

			UpdateStyleIfNeeded();
			UpdateLayoutIfNeeded();

			SetNeedsLayout();
			SetNeedsStyle();
			SetNeedsPaint();
		}
	}

	void CPopup::Hide()
	{
#if PAL_TRAIT_BUILD_STANDALONE
		showNativeWindow = false;
#endif

		SetEnabled(false);

		if (nativeWindow)
		{
			nativeWindow->GetPlatformWindow()->SetHitTestDelegate(nullptr);

			delete nativeWindow;
			nativeWindow = nullptr;

			SetNeedsLayout();
			SetNeedsStyle();
			SetNeedsPaint();
		}
		else if (rootWindow)
		{
			rootWindow->DetachSubWindow(this);

			SetNeedsLayout();
			SetNeedsStyle();
			SetNeedsPaint();

			rootWindow = nullptr;
		}
	}

	void CPopup::Show(Vec2i screenPosition, Vec2i size)
	{
		showPosition = screenPosition;
		if (!showNativeWindow)
		{
			rootWindow = GetRootWindow();
			if (rootWindow && rootWindow->GetNativeWindow())
			{
				Vec2i windowPos = rootWindow->GetNativeWindow()->platformWindow->GetWindowPosition();
				showPosition -= windowPos;
			}
		}

		showSize = size;
		windowSize = showSize.ToVec2();

		Show();
	}

	bool CPopup::IsShown()
	{
		return IsEnabled();
	}

	Vec2 CPopup::GetComputedLayoutTopLeft()
	{
		if (IsShown() && !showNativeWindow && rootWindow && rootWindow->GetNativeWindow())
		{
			return showPosition.ToVec2();
			//Vec2i windowPos = rootWindow->GetNativeWindow()->platformWindow->GetWindowPosition();
			//return (showPosition - windowPos).ToVec2();
		}
		return Super::GetComputedLayoutTopLeft();
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
