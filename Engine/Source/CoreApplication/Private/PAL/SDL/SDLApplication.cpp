
#include "CoreApplication.h"

#include <SDL.h>

namespace CE
{
	static int ResizingEventWatch(void* data, SDL_Event* event);

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

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
		{
			CE_LOG(Error, All, "Failed to initialize SDL Video & Audio! " + String(SDL_GetError()));
		}
	}

	void SDLApplication::PreShutdown()
	{
		Super::PreShutdown();

		DestroyWindow(mainWindow);
	}

	void SDLApplication::Shutdown()
	{
		Super::Shutdown();

		SDL_Quit();
	}

	PlatformWindow* SDLApplication::InitMainWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen, bool resizable)
	{
		if (mainWindow == nullptr)
		{
			mainWindow = new SDLPlatformWindow(title, width, height, maximised, fullscreen, resizable);
			windowList.Add(mainWindow);

			SDL_AddEventWatch(ResizingEventWatch, mainWindow->handle);
		}
		return mainWindow;
	}

	PlatformWindow* SDLApplication::GetMainWindow()
	{
		return mainWindow;
	}

	PlatformWindow* SDLApplication::CreatePlatformWindow(const String& title)
	{
		if (mainWindow == nullptr)
		{
			return InitMainWindow(title, gDefaultWindowWidth, gDefaultWindowHeight, false, false);
		}
		auto window = new SDLPlatformWindow(title, gDefaultWindowWidth, gDefaultWindowHeight, false, false);
		windowList.Add(window);
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
		return window;
	}

	void SDLApplication::DestroyWindow(PlatformWindow* window)
	{
		if (window == nullptr)
			return;

		auto sdlWindow = (SDLPlatformWindow*)window;

		if (sdlWindow->GetWindowId() == mainWindow->GetWindowId())
		{
			SDL_DelEventWatch(ResizingEventWatch, mainWindow->handle);
			this->mainWindow = nullptr;
		}

		windowList.Remove(sdlWindow);
		delete sdlWindow;
	}

	void SDLApplication::Tick()
	{
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

			
		}

		Super::Tick();
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
            onWindowResized.Broadcast(window, w, h);
		}
	}

	int ResizingEventWatch(void* data, SDL_Event* event)
	{
		if (event->type == SDL_WINDOWEVENT &&
			event->window.event == SDL_WINDOWEVENT_RESIZED) 
		{
			SDL_Window* sdlWindow = SDL_GetWindowFromID(event->window.windowID);
			if (sdlWindow == (SDL_Window*)data)
			{

			}
		}
		return 0;
	}
}
