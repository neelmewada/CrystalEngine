#pragma once

#include "EngineDefs.h"

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

struct SDL_Window;

namespace Vox
{

class ENGINE_API ApplicationBase
{
public:
    ApplicationBase(std::string windowTitle, bool maximised, bool fullscreen, int w, int h);
    ~ApplicationBase();

    ApplicationBase(const ApplicationBase&) = delete;
    ApplicationBase& operator=(const ApplicationBase&) = delete;

public: // Public API
    virtual void Start();
    virtual void Run();
    static ApplicationBase* Instance() { return m_Instance; }

public: // Getters
    std::string GetTitle() { return m_Title; }
    SDL_Window* GetWindow() { return m_Window; }

protected: // Abstract Functions
    virtual void PollEvent(SDL_Event) = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;

public: // Public Abstract Functions
    virtual const char* GetApplicationName() = 0;
    virtual uint32_t GetApplicationVersion() = 0;

protected: // Internal Functions

protected: // Internal Members
    std::string m_Title;
    SDL_Window* m_Window;
    static ApplicationBase* m_Instance;

    float m_DeltaTime = 0;
};

}


