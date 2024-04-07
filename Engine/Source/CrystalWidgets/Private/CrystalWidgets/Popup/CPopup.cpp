#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CPopup::CPopup()
	{
		allowVerticalScroll = false;
		allowHorizontalScroll = false;

		if (PlatformApplication::TryGet())
			PlatformApplication::TryGet()->AddMessageHandler(this);
	}

	CPopup::~CPopup()
	{
		if (PlatformApplication::TryGet())
			PlatformApplication::TryGet()->RemoveMessageHandler(this);
	}

	void CPopup::OnBeforeDestroy()
	{
		//Hide();
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
		//nativeWindow->SetBorderless(true);
		nativeWindow->Show();

		ConstructWindow();

		CApplication::Get()->windows.Add(this);
	}

	void CPopup::Hide()
	{
		if (!nativeWindow)
			return;
		
		PlatformApplication::Get()->DestroyWindow(nativeWindow);
		nativeWindow = nullptr;

		CApplication::Get()->windows.Remove(this);

		if (swapChain)
		{
			RHI::gDynamicRHI->DestroySwapChain(swapChain);
			swapChain = nullptr;
		}
	}

	void CPopup::Show(Vec2i screenPosition, Vec2i size)
	{
		showPosition = screenPosition;
		showSize = size;
		windowSize = showSize.ToVec2();

		Show();
	}

	void CPopup::OnWindowDestroyed(PlatformWindow* window)
	{
		if (window == nativeWindow)
		{
			nativeWindow = nullptr;
		}
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
