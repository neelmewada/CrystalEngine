
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
        for (int i = 0; i < components.GetSize(); i++)
        {
            if (components[i]->IsActive())
            {
                components[i]->Init();
                components[i]->Activate();
            }
        }
    }

    void Engine::Shutdown()
    {
        
    }

    void Engine::Tick(f32 deltaTime)
    {
        MBUS_EVENT(SystemTickBus, OnTick, deltaTime);

        for (int i = 0; i < components.GetSize(); i++)
        {
            if (components[i] == nullptr)
                continue;

            if (components[i]->IsActive())
                components[i]->Tick(deltaTime);
        }
    }

    void Engine::OnSystemComponentAdded(SystemComponent* component)
    {
        component->Init();
        component->Activate();
    }

    void Engine::OnSystemComponentRemoved(SystemComponent* component)
    {
        component->Deactivate();
    }

} // namespace CE

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, Engine)

