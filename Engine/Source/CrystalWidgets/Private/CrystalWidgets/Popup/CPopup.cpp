#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CPopup::CPopup()
	{
		allowVerticalScroll = false;
		allowHorizontalScroll = false;
	}

	CPopup::~CPopup()
	{

	}

	void CPopup::OnBeforeDestroy()
	{
		//Hide();
	}

	bool CPopup::IsSubWidgetAllowed(Class* subWidgetClass)
	{
		return subWidgetClass->IsSubclassOf<CWidget>();
	}

	void CPopup::Show()
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

		nativeWindow->platformWindow->SetWindowPosition(showPosition);
		nativeWindow->platformWindow->SetBorderless(true);
		nativeWindow->platformWindow->SetAlwaysOnTop(true);
		nativeWindow->Show();

		nativeWindow->platformWindow->SetInputFocus();
	}

	void CPopup::Hide()
	{
		if (!nativeWindow)
			return;
		
		delete nativeWindow;
		nativeWindow = nullptr;
	}

	void CPopup::Show(Vec2i screenPosition, Vec2i size)
	{
		showPosition = screenPosition;
		showSize = size;
		windowSize = showSize.ToVec2();

		Show();
	}

	void CPopup::HandleEvent(CEvent* event)
	{
		if (event->type == CEventType::FocusChanged)
		{
			CFocusEvent* focusEvent = (CFocusEvent*)event;
		}

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
