
#include "System/SystemApplication.h"

namespace CE
{

    SystemApplication* SystemApplication::Singleton = nullptr;
    
    SystemApplication::SystemApplication() : SystemApplication(0, nullptr)
    {
        
    }

    SystemApplication::SystemApplication(int argc, char** argv)
    {
        SystemApplicationBus::BusConnect(this);
    }

    SystemApplication::~SystemApplication()
    {
        SystemApplicationBus::BusDisconnect(this);
    }

} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, SystemApplication)

