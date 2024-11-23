
#include "CoreApplication.h"

#include "PAL/Common/PlatformWindowMisc.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <AppKit/AppKit.h>

namespace CE
{

    u32 MacWindowMisc::GetDpiForWindow(SDLPlatformWindow* window)
    {
        if (window == nullptr)
        {
            return PlatformApplication::Get()->GetSystemDpi();
        }

        auto handle = (SDL_Window*)window->GetUnderlyingHandle();

        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(handle, &wmInfo);
        
        // Get the backing scale factor
        CGFloat scaleFactor = [wmInfo.info.cocoa.window backingScaleFactor]; // Alternatively, screen.backingScaleFactor

        // Calculate DPI
        CGFloat logicalDPI = 72.0; // macOS logical DPI
        CGFloat effectiveDPI = logicalDPI * scaleFactor;
        
        return (u32)effectiveDPI;
    }

} // namespace CE
