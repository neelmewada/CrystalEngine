
#include "GameFramework/Engine.h"

#include "Component/TickBus.h"

namespace CE
{
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

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, Engine)

