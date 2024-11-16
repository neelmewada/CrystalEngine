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

    private:

        WindowsSDLApplication();

    };

    typedef WindowsSDLApplication PlatformApplicationImpl;

}
