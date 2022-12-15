
#include "GameFramework/Engine.h"

#include "Component/TickBus.h"

namespace CE
{
    
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

} // namespace CE

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, Engine)

