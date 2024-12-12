

#include "CoreApplication.h"

#include <Windows.h>
#include <windowsx.h>
#include <shellscalingapi.h>

#pragma comment(lib, "shcore.lib")

#include <SDL.h>
#include <SDL_syswm.h>

namespace CE
{
    WindowsSDLApplication* WindowsSDLApplication::Create()
    {
        return new WindowsSDLApplication();
    }

    void WindowsSDLApplication::Initialize()
    {
        SetProcessDPIAware();

        Super::Initialize();
    }

    u32 WindowsSDLApplication::GetSystemDpi()
    {
        return GetDpiForSystem();
    }

    WindowsSDLApplication::WindowsSDLApplication()
    {

    }

	int SDLWindowEventWatch(void* data, SDL_Event* event)
	{
		if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_EXPOSED)
		{
			auto app = SDLApplication::Get();

			for (SDLPlatformWindow* window : app->windowList)
			{
				if (window->GetWindowId() == event->window.windowID)
				{
					for (ApplicationMessageHandler* messageHandler : app->messageHandlers)
					{
						messageHandler->OnWindowExposed(window);
					}

					break;
				}
			}

			for (const auto& tickHandler : app->tickHanders)
			{
				tickHandler.InvokeIfValid();
			}
		}
		else if (event->syswm.msg != nullptr && event->syswm.type == SDL_SYSWM_WINDOWS)
		{
			auto app = SDLApplication::Get();

			for (SDLPlatformWindow* window : app->windowList)
			{
				if (window->GetWindowId() == event->window.windowID)
				{
					auto windowsEvent = event->syswm.msg->msg.win;

					switch (windowsEvent.msg)
					{
					case WM_NCHITTEST:
					{
						POINT point = { GET_X_LPARAM(windowsEvent.lParam), GET_Y_LPARAM(windowsEvent.lParam) };

					}
						break;
					}

					break;
				}
			}
		}
		return 0;
	}

}
