
#include "System.h"
#include "CoreMinimal.h"

#include "GameFramework/GameFramework.h"

CE_IMPLEMENT_MODULE(System, CE::SystemModule)

namespace CE
{
    SYSTEM_API Engine* GEngine = nullptr;
    
    void SystemModule::StartupModule()
    {
        GEngine = new Engine();
    }

    void SystemModule::ShutdownModule()
    {
        delete GEngine;
    }

    void SystemModule::RegisterTypes()
    {
        CE_REGISTER_TYPES(
            Engine,
            GameObject,
            GameComponent
        );
    }

} // namespace CE

