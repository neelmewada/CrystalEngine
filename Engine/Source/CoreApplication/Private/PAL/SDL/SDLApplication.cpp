
#include "CoreApplication.h"

#include <SDL.h>

namespace CE
{
	static int SDLWindowEventWatch(void* data, SDL_Event* event);

	SDLApplication* SDLApplication::Create()
	{
		return new SDLApplication();
	}

	SDLApplication* SDLApplication::Get()
	{
		return (SDLApplication*)PlatformApplication::Get();
	}

	SDLApplication::SDLApplication()
	{
		
	}

	SDLApplication::~SDLApplication()
	{

	}

	void SDLApplication::Initialize()
	{
		Super::Initialize();
		
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0)
		{
			CE_LOG(Error, All, "Failed to initialize SDL Video & Audio! {}", SDL_GetError());
		}

		SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
		SDL_EventState(SDL_DROPTEXT, SDL_ENABLE);
	}

	void SDLApplication::PreShutdown()
	{
		Super::PreShutdown();

		if (mainWindow != nullptr)
			DestroyWindow(mainWindow);
	}

	void SDLApplication::Shutdown()
	{
		Super::Shutdown();

		for (int i = 0; i < systemCursors.GetSize(); ++i)
		{
			if (systemCursors[i] != nullptr)
			{
				SDL_FreeCursor(systemCursors[i]);
				systemCursors[i] = nullptr;
			}
		}

		SDL_Quit();
	}

	void SDLApplication::SetSystemCursor(SystemCursor cursor)
	{
		if (systemCursors[(int)cursor] == nullptr)
		{
			systemCursors[(int)cursor] = SDL_CreateSystemCursor((SDL_SystemCursor)cursor);
		}

		SDL_SetCursor(systemCursors[(int)cursor]);
	}

	PlatformWindow* SDLApplication::InitMainWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen, bool resizable)
	{
		if (mainWindow == nullptr)
		{
			mainWindow = new SDLPlatformWindow(title, width, height, maximised, fullscreen, resizable);
			mainWindow->isMainWindow = true;
			windowList.Add(mainWindow);

			SDL_AddEventWatch(SDLWindowEventWatch, mainWindow->handle);
		}

		for (auto messageHandler : messageHandlers)
		{
			messageHandler->OnWindowCreated(mainWindow);
		}
		return mainWindow;
	}

	PlatformWindow* SDLApplication::GetMainWindow()
	{
		return mainWindow;
	}

	PlatformWindow* SDLApplication::FindWindow(u64 windowId)
	{
		for (auto window : windowList)
		{
			if (window->GetWindowId() == windowId)
			{
				return window;
			}
		}

		return nullptr;
	}

	PlatformWindow* SDLApplication::CreatePlatformWindow(const String& title)
	{
		if (mainWindow == nullptr)
		{
			return InitMainWindow(title, gDefaultWindowWidth, gDefaultWindowHeight, false, false);
		}
		auto window = new SDLPlatformWindow(title, gDefaultWindowWidth, gDefaultWindowHeight, false, false);
		windowList.Add(window);
		for (auto messageHandler : messageHandlers)
		{
			messageHandler->OnWindowCreated(window);
		}
		return window;
	}

	PlatformWindow* SDLApplication::CreatePlatformWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen)
	{
		if (mainWindow == nullptr)
		{
			return InitMainWindow(title, width, height, maximised, fullscreen);
		}
		auto window = new SDLPlatformWindow(title, width, height, maximised, fullscreen);
		windowList.Add(window);
		for (auto messageHandler : messageHandlers)
		{
			messageHandler->OnWindowCreated(window);
		}
		return window;
	}

	Vec2i SDLApplication::GetMainScreenSize()
	{
		return GetScreenSizeForWindow(mainWindow);
	}

	Vec2i SDLApplication::GetScreenSizeForWindow(PlatformWindow* window)
	{
		SDLPlatformWindow* sdlWindow = (SDLPlatformWindow*)window;

		SDL_DisplayMode mode{};
		int displayIndex = 0;
		if (sdlWindow != nullptr)
			displayIndex = SDL_GetWindowDisplayIndex(sdlWindow->handle);
		
		if (SDL_GetDesktopDisplayMode(displayIndex, &mode) != 0)
		{
			CE_LOG(Error, All, "Failed to get screen size for window. Error: {}", SDL_GetError());
			return Vec2i();
		}
		return Vec2i(mode.w, mode.h);
	}

	Vec2i SDLApplication::GetWindowSize(void* nativeWindowHandle)
	{
		SDL_Window* window = (SDL_Window*)nativeWindowHandle;
		if (window == nullptr)
			return Vec2i();

		Vec2i result{};
		SDL_GetWindowSize(window, &result.width, &result.height);
		return result;
	}

	void SDLApplication::DestroyWindow(PlatformWindow* window)
	{
		if (window == nullptr)
			return;
		
		auto sdlWindow = (SDLPlatformWindow*)window;

		windowList.Remove(sdlWindow);

		if (sdlWindow->GetWindowId() == mainWindow->GetWindowId())
		{
			SDL_DelEventWatch(SDLWindowEventWatch, mainWindow->handle);

			// Destroy all windows (i.e. shutdown application) if main window is destroyed
			for (int i = windowList.GetSize() - 1; i >= 0; --i)
			{
				if (windowList[i] == mainWindow)
				{
					windowList.RemoveAt(i);
					continue;
				}
				DestroyWindow(windowList[i]);
			}

			RequestEngineExit("MAIN_WINDOW_CLOSED");
			this->mainWindow = nullptr;
		}

		for (ApplicationMessageHandler* handler : messageHandlers)
		{
			handler->OnWindowDestroyed(window);
		}

		delete sdlWindow;
	}

	void SDLApplication::Tick()
	{
		PlatformApplication::Tick();

		// Handle SDL Events
		SDL_Event sdlEvent;
		while (SDL_PollEvent(&sdlEvent))
		{
			for (auto handler : messageHandlers)
			{
				if (handler != nullptr)
				{
					handler->ProcessNativeEvents(&sdlEvent);
				}
			}

			if (sdlEvent.type == SDL_QUIT)
			{
				RequestEngineExit("APP_QUIT");
				break;
			}
			
			if (sdlEvent.type == SDL_WINDOWEVENT)
			{
				ProcessWindowEvents(sdlEvent);
			}
			
			if (sdlEvent.type == SDL_KEYDOWN || sdlEvent.type == SDL_KEYUP ||
				sdlEvent.type == SDL_MOUSEMOTION || sdlEvent.type == SDL_MOUSEBUTTONDOWN || sdlEvent.type == SDL_MOUSEBUTTONUP || sdlEvent.type == SDL_MOUSEWHEEL)
			{
				ProcessInputEvents(sdlEvent);
			}
		}

		Super::Tick();
	}

	bool SDLApplication::HasClipboardText()
	{
		return SDL_HasClipboardText();
	}

	String SDLApplication::GetClipboardText()
	{
		char* cString = SDL_GetClipboardText();
		String string = cString;
		SDL_free(cString);
		return string;
	}

	void SDLApplication::SetClipboardText(const String& text)
	{
		SDL_SetClipboardText(text.GetCString());
	}

	void SDLApplication::ProcessWindowEvents(SDL_Event& event)
	{
		if (event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
            if (event.window.windowID == mainWindow->GetWindowId())
            {
                ProcessWindowResizeEvent(mainWindow);
            }
            else
            {
                for (PlatformWindow* window : windowList)
                {
                    if (event.window.windowID == window->GetWindowId())
                    {
                        ProcessWindowResizeEvent((SDLPlatformWindow*)window);
                        break;
                    }
                }
            }
		}
		else if (event.window.event == SDL_WINDOWEVENT_CLOSE) // Close a specific window
		{
			for (PlatformWindow* window : windowList)
			{
				if (event.window.windowID == window->GetWindowId())
				{
					DestroyWindow(window);
					break;
				}
			}
		}
		else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
		{
			for (auto window : windowList)
			{
				if ((u32)window->GetWindowId() == event.window.windowID) // Found the minimized window
				{
					for (ApplicationMessageHandler* handler : messageHandlers)
					{
						handler->OnWindowMinimized(window);
					}
					break;
				}
			}
		}
		else if (event.window.event == SDL_WINDOWEVENT_RESTORED)
		{
			for (auto window : windowList)
			{
				if ((u32)window->GetWindowId() == event.window.windowID) // Found the minimized window
				{
					for (ApplicationMessageHandler* handler : messageHandlers)
					{
						handler->OnWindowRestored(window);
					}
					break;
				}
			}
		}
	}

	void SDLApplication::ProcessInputEvents(SDL_Event& event)
	{
		for (auto handler : messageHandlers)
		{
			handler->ProcessInputEvents(&event);
		}
	}

	void SDLApplication::ProcessWindowResizeEvent(SDLPlatformWindow* window)
	{
		u32 w = 0, h = 0;
        window->GetDrawableWindowSize(&w, &h);

		if (w != 0 && h != 0)
		{
			for (auto handler : messageHandlers)
			{
				if (handler != nullptr && window->GetWindowId() == mainWindow->GetWindowId())
					handler->OnMainWindowDrawableSizeChanged(w, h);
			}
            onWindowDrawableSizeChanged.Broadcast(window, w, h);

			for (ApplicationMessageHandler* handler : messageHandlers)
			{
				handler->OnWindowResized(window, w, h);
			}
		}
	}

	int SDLWindowEventWatch(void* data, SDL_Event* event)
	{
#if PLATFORM_WINDOWS
		if (event->type == SDL_WINDOWEVENT &&
			event->window.event == SDL_WINDOWEVENT_MOVED) 
		{
			auto app = SDLApplication::Get();
			for (const auto& tickHandler : app->tickHanders)
			{
				tickHandler.InvokeIfValid();
			}
		}
		
#endif
		return 0;
	}
}
