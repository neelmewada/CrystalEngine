
#include "CoreApplication.h"

#include <CoreGraphics/CoreGraphics.h>
#include <AppKit/AppKit.h>

namespace CE
{
    MacSDLApplication* MacSDLApplication::Create()
    {
        return new MacSDLApplication();
    }

    void MacSDLApplication::Initialize()
    {
        [[NSUserDefaults standardUserDefaults] setBool: YES
            forKey: @"AppleMomentumScrollSupported"];

        Super::Initialize();

    }

    u32 MacSDLApplication::GetSystemDpi()
    {
        NSScreen *mainScreen = [NSScreen mainScreen];

        // Get the scale factor (Retina scaling)
        CGFloat scale = [mainScreen backingScaleFactor];
        //std::cout << "Scale Factor (Retina): " << scale << std::endl;

        return (u32)(scale * 72.0f);
    }

    MacSDLApplication::MacSDLApplication()
    {

    }

    int SDLWindowEventWatch(void* data, SDL_Event* event)
    {
        
        return 0;
    }

}
