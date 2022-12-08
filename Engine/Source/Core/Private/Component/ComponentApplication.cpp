
#include "Component/ComponentApplication.h"

namespace CE
{

    IComponentApplication::IComponentApplication() : IComponentApplication(0, nullptr)
    {

    }

    IComponentApplication::IComponentApplication(int argc, char** argv)
    {
        ComponentApplicationBus::BusConnect(this);
    }

    IComponentApplication::~IComponentApplication()
    {
        ComponentApplicationBus::BusDisconnect(this);
    }

    //////////////////////////////////////////////////////////////////////////
    // ComponentApplicationRequests

    IComponentApplication* IComponentApplication::GetApplication()
    {
        return this;
    }

    //////////////////////////////////////////////////////////////////////////
    // TickRequests

    f32 IComponentApplication::GetTickDeltaTime()
    {
        return 0;
    }

    //////////////////////////////////////////////////////////////////////////

    void IComponentApplication::Tick()
    {
        MBUS_EVENT(TickBus, OnTick, (f32)(clock() - PrevClock) / CLOCKS_PER_SEC);
        PrevClock = clock();
    }

} // namespace CE

