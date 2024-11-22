
#include "CoreApplication.h"

#include "PAL/Common/PlatformWindowMisc.h"

#include <SDL2/SDL.h>

#include <Windows.h>
#include <shellscalingapi.h>

namespace CE
{

    u32 WindowsWindowMisc::GetDpiForWindow(SDLPlatformWindow* window)
    {
        if (window == nullptr)
            return GetDpiForSystem();

        HWND hWnd = (HWND)window->GetOSNativeHandle();
        return ::GetDpiForWindow(hWnd);
    }

} // namespace CE
