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

        float GetDisplayScaling() override { return displayScaling; }

    private:

        LinuxSDLApplication();

        void CalculateDPI();

        float displayScaling = 1;
    };

    typedef LinuxSDLApplication PlatformApplicationImpl;

}
