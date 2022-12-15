
#include "Component/ComponentApplication.h"

namespace CE
{

    ComponentApplication::ComponentApplication() : ComponentApplication(0, nullptr)
    {

    }

    ComponentApplication::ComponentApplication(int argc, char** argv)
    {
        ComponentApplicationBus::BusConnect(this);
    }

    ComponentApplication::~ComponentApplication()
    {
        ComponentApplicationBus::BusDisconnect(this);
    }

    //////////////////////////////////////////////////////////////////////////
    // ComponentApplicationRequests

    ComponentApplication* ComponentApplication::GetApplication()
    {
        return this;
    }

    //////////////////////////////////////////////////////////////////////////
    // TickRequests

    f32 ComponentApplication::GetTickDeltaTime()
    {
        return TickDeltaTime;
    }

    //////////////////////////////////////////////////////////////////////////

    void ComponentApplication::Tick()
    {
        TickDeltaTime = (f32)(clock() - PrevClock) / CLOCKS_PER_SEC;
        MBUS_EVENT(TickBus, OnTick, TickDeltaTime);
        PrevClock = clock();
    }

    void ComponentApplication::TickSystem()
    {
        
    }

} // namespace CE

