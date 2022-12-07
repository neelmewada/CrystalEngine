
#include "System/SystemApplication.h"

namespace CE
{

    SystemApplication* SystemApplication::Singleton = nullptr;
    
    SystemApplication::SystemApplication() : SystemApplication(0, nullptr)
    {
        
    }

    SystemApplication::SystemApplication(int argc, char** argv)
    {
        Singleton = this;

        SystemApplicationBus::BusConnect(this);
    }

    SystemApplication::~SystemApplication()
    {
        Singleton = nullptr;

        SystemApplicationBus::BusDisconnect(this);
    }

} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, SystemApplication)

