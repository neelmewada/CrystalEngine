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
		useNativeWindow = true;
#else
		showNativeWindow = false;
#endif

		// TODO: Testing only
		useNativeWindow = false;
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

		if (useNativeWindow)
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

			// FIXME: Temporary hack, need a better focus/un-focus logic
			if (!IsOfType<CMenu>())
			{
				Focus();
			}
		}
		else
		{
			rootWindow = GetRootWindow();
			if (!rootWindow)
				return;

			rootWindow->AttachSubWindow(this);
			
			UpdateStyleIfNeeded();
			UpdateLayoutIfNeeded();

			SetNeedsLayout();
			SetNeedsStyle();
			SetNeedsPaint();

			// FIXME: Temporary hack, need a better focus/un-focus logic
			if (!IsOfType<CMenu>())
			{
				Focus();
			}
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
		if (!useNativeWindow)
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
		if (IsShown() && !useNativeWindow && rootWindow && rootWindow->GetNativeWindow())
		{
			return showPosition.ToVec2();
		}
		return Super::GetComputedLayoutTopLeft();
	}

	Vec2 CPopup::GetComputedLayoutSize()
	{
		return Super::GetComputedLayoutSize();
	}

	bool CPopup::PostComputeStyle()
	{
		bool layoutUpdated = Super::PostComputeStyle();

		if (showSize.width > 0.1f)
		{
			YGNodeStyleSetMinWidth(node, showSize.width);
			return true;
		}

		return layoutUpdated;
	}

	void CPopup::OnFocusLost()
	{
		Super::OnFocusLost();

		if (hideWhenUnfocused)
		{
			Hide();
		}
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
