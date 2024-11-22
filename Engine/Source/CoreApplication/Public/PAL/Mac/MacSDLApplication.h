#pragma once

#include "PAL/SDL/SDLApplication.h"

namespace CE
{

    class COREAPPLICATION_API MacSDLApplication final : public SDLApplication
    {
    public:
        typedef SDLApplication Super;

        static MacSDLApplication* Create();

        void Initialize() override;

    private:

        MacSDLApplication();

    };

    typedef MacSDLApplication PlatformApplicationImpl;
    
}