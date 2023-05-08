
#include "CoreApplication.h"

#include <SDL.h>

namespace CE
{

	SDLApplication* SDLApplication::Create()
	{
		return new SDLApplication();
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

		SDL_Init(SDL_INIT_VIDEO);
	}

	void SDLApplication::PreShutdown()
	{
		Super::PreShutdown();
	}

	void SDLApplication::Shutdown()
	{
		Super::Shutdown();

		SDL_Quit();
	}

	PlatformWindow* SDLApplication::InitMainWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen)
	{
		if (mainWindow == nullptr)
			mainWindow = new SDLPlatformWindow(title, width, height, maximised, fullscreen);
		return mainWindow;
	}

	PlatformWindow* SDLApplication::GetMainWindow()
	{
		return mainWindow;
	}

	void SDLApplication::Tick()
	{
		// Handle SDL Events
		SDL_Event sdlEvent;
		while (SDL_PollEvent(&sdlEvent))
		{
			if (messageHandler != nullptr)
			{
				//messageHandler->PreprocessNativeEvent(&SDLEvent);
			}

			if (sdlEvent.type == SDL_QUIT)
			{
				RequestEngineExit("USER_QUIT");
				break;
			}

			if (sdlEvent.type == SDL_WINDOWEVENT)
			{
				
			}

			
		}

		Super::Tick();
	}

}
