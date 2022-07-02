
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "ApplicationBase.h"
#include <iostream>

using namespace Vox;

ApplicationBase* ApplicationBase::m_Instance = nullptr;

ApplicationBase::ApplicationBase(std::string windowTitle, bool maximised, bool fullscreen, int w, int h)
{
    m_Title = windowTitle;
    m_Instance = this;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);

    if (maximised)
        windowFlags = (SDL_WindowFlags)(windowFlags | SDL_WINDOW_MAXIMIZED);
    if (fullscreen)
        windowFlags = (SDL_WindowFlags)(windowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);

    m_Window = SDL_CreateWindow(m_Title.c_str(),
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                w, h, windowFlags);
}

ApplicationBase::~ApplicationBase()
{
    SDL_DestroyWindow(m_Window);

    m_Instance = nullptr;
}

void ApplicationBase::Start()
{
    
}

void ApplicationBase::Run()
{
    SDL_Event e;
    bool quit = false;

    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) quit = true;
            if (e.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                // Window resized
                std::cout << "Window Resized" << std::endl;
            }
            PollEvent(e);
        }

        Update();
        Render();
    }
}
