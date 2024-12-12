#pragma once

namespace CE
{

    class LinuxWindowMisc
    {
        CE_STATIC_CLASS(LinuxWindowMisc)
    public:

        static u32 GetDpiForWindow(SDLPlatformWindow* sdlWindow);

        static void SetupBorderlessWindow(SDLPlatformWindow* window, bool borderless) {}

    };

    typedef LinuxWindowMisc PlatformWindowMisc;

} // namespace CE
