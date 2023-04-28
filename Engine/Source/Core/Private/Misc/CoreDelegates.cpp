
#include "CoreMinimal.h"

namespace CE
{

    CoreDelegates::ModuleLoadDelegate CoreDelegates::onBeforeModuleLoad{};
    CoreDelegates::ModuleLoadDelegate CoreDelegates::onAfterModuleLoad{};

    CoreDelegates::ModuleLoadDelegate CoreDelegates::onBeforeModuleUnload{};
    CoreDelegates::ModuleLoadDelegate CoreDelegates::onAfterModuleUnload{};
    
    CoreDelegates::ModuleLoadFailedDelegate CoreDelegates::onModuleFailedToLoad{};

} // namespace CE

