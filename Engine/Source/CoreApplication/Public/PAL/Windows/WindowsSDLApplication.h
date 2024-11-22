#pragma once

#include "PAL/SDL/SDLApplication.h"

namespace CE
{

    class COREAPPLICATION_API WindowsSDLApplication final : public SDLApplication
    {
    public:
        typedef SDLApplication Super;

        static WindowsSDLApplication* Create();

        void Initialize() override;

        u32 GetSystemDpi() override;

    private:

        WindowsSDLApplication();

        float displayScaling = 0;
        float dpi = 0;
    };

    typedef WindowsSDLApplication PlatformApplicationImpl;

}
