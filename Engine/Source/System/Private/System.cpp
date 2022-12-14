
#include "System.h"
#include "CoreMinimal.h"

#include "GameFramework/GameFramework.h"

CE_IMPLEMENT_MODULE(System, CE::SystemModule)

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
            Application,
            GameObject,
            GameComponent
        );
    }

} // namespace CE

