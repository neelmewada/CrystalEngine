
#include "GameFramework/Engine.h"

#include "Component/TickBus.h"

namespace CE
{
    SYSTEM_API Engine* gEngine = nullptr;

    Engine::Engine()
    {

    }

    Engine::~Engine()
    {

    }

    void Engine::Startup()
    {
        
    }

    void Engine::Shutdown()
    {
        
    }

    void Engine::Tick(f32 deltaTime)
    {
        MBUS_EVENT(SystemTickBus, OnTick, deltaTime);

        
    }

} // namespace CE


