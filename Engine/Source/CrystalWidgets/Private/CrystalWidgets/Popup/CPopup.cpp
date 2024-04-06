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

		nativeWindow = PlatformApplication::Get()->CreatePlatformWindow(title, showSize.width, showSize.height, windowInfo);
		nativeWindow->SetWindowPosition(showPosition);
		nativeWindow->Show();
	}

	void CPopup::Hide()
	{
		if (!nativeWindow)
			return;

		PlatformApplication::Get()->DestroyWindow(nativeWindow);
		nativeWindow = nullptr;
	}

	void CPopup::Show(Vec2i screenPosition, Vec2i size)
	{
		showPosition = screenPosition;
		showSize = size;
		windowSize = showSize.ToVec2();

		Show();
	}

} // namespace CE::Widgets
