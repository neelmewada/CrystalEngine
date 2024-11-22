#pragma once

#include "Core.h"

#include "PAL/Common/PlatformApplication.h"
#include "PAL/Common/PlatformWindow.h"

#if PAL_TRAIT_SDL_SUPPORTED

#if PLATFORM_WINDOWS
#   include "PAL/Windows/WindowsSDLApplication.h"
#elif PLATFORM_MAC
#   include "PAL/Mac/MacSDLApplication.h"
#elif PLATFORM_LINUX
#   include "PAL/Linux/LinuxSDLApplication.h"
#else
#   include "PAL/SDL/SDLApplication.h"
#endif

#include "PAL/SDL/SDLWindow.h"
#else
#error SDL not supported! Currently, the engine only supports SDL as the application & windowing platform.
#endif

#include "Input/CoreInputTypes.h"
#include "Input/InputManager.h"

#include "Input/PAL/Common/PlatformInput.h"
#if PAL_TRAIT_SDL_SUPPORTED
#include "Input/PAL/SDL/SDLPlatformInput.h"
#endif

namespace CE
{
    
    class COREAPPLICATION_API CoreApplicationModule : public CE::Module
    {
    public:
        virtual void StartupModule() override;

        virtual void ShutdownModule() override;

        virtual void RegisterTypes() override;
    };

}