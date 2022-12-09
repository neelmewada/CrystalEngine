
#include "System.h"
#include "CoreMinimal.h"

namespace CE
{
    
    void SystemModule::StartupModule()
    {

    }

    void SystemModule::ShutdownModule()
    {

    }

    void SystemModule::RegisterTypes()
    {
        CE_REGISTER_TYPES(
            Application
        );
    }

} // namespace CE
