#pragma once

#include "Delegates/MultiCastDelegate.h"

namespace CE
{

    struct CORE_API CoreDelegates
    {
        CoreDelegates() = delete;

        /* *******************************************
        *   Modules
        */

        typedef MultiCastDelegate<void(ModuleInfo*)> ModuleLoadDelegate;

        static ModuleLoadDelegate onBeforeModuleLoad;
        static ModuleLoadDelegate onAfterModuleLoad;

        static ModuleLoadDelegate onBeforeModuleUnload;
        static ModuleLoadDelegate onAfterModuleUnload;

        typedef MultiCastDelegate<void(const String&, ModuleLoadResult)> ModuleLoadFailedDelegate;

        static ModuleLoadFailedDelegate onModuleFailedToLoad;

#if PAL_TRAIT_WITH_EDITOR_DATA
        
#endif
    };
    
} // namespace CE
