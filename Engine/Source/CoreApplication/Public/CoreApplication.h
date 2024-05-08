#pragma once

#include "Core.h"

#include "PAL/Common/PlatformApplication.h"
#include "PAL/Common/PlatformWindow.h"

#if PAL_TRAIT_SDL_SUPPORTED
#include "PAL/SDL/SDLApplication.h"
#include "PAL/SDL/SDLWindow.h"
#else
#error SDL not supported! Currently, the engine only supports SDL as the application & windowing platform.
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