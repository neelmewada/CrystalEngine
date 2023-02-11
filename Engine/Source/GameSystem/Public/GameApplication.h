#pragma once

#include "CoreMinimal.h"
#include "System.h"

namespace CE::Game
{

    class GAMESYSTEM_API GameApplication
        : public Application
    {
        CE_CLASS(GameApplication, Application);

    public:

        GameApplication();
        GameApplication(int argc, char** argv);

        virtual ~GameApplication();

    };
    
} // namespace CE::Game


CE_RTTI_CLASS(GAMESYSTEM_API, CE::Game, GameApplication,
    CE_SUPER(CE::Application),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)

