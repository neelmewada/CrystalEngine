
#include "GameApplication.h"

namespace CE::Game
{
    
    GameApplication::GameApplication() : GameApplication(0, nullptr)
    {
    }

    GameApplication::GameApplication(int argc, char** argv) : Application(argc, argv)
    {

    }

    GameApplication::~GameApplication()
    {

    }

} // namespace CE::Game
