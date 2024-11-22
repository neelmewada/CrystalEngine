#pragma once

#include "PAL/SDL/SDLApplication.h"

namespace CE
{

    class COREAPPLICATION_API LinuxSDLApplication final : public SDLApplication
    {
    public:
        typedef SDLApplication Super;

        static LinuxSDLApplication* Create();

        void Initialize() override;

        u32 GetSystemDpi() override;

    private:

        LinuxSDLApplication();

        void CalculateDPI();

        float displayScaling = 1;
        u32 dpi = 0;
    };

    typedef LinuxSDLApplication PlatformApplicationImpl;

}
