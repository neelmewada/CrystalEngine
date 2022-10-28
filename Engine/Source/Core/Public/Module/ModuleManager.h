#pragma once

#include "Misc/CoreDefines.h"
#include "Containers/String.h"
#include "Containers/HashMap.h"

#include "Module.h"

namespace CE
{

    struct ModuleStatusInfo
    {
        String ModuleName;
        Module* ModuleImpl;
        
        void* DllHandle;

        bool bIsLoaded;
    };
    
    class CORE_API ModuleManager
    {
    private:
        ModuleManager();
        ~ModuleManager();

    public:

        CE_SINGLETON_CLASS(ModuleManager);

        static ModuleManager& Get()
        {
            static ModuleManager instance;
            return instance;
        }

        

    };

} // namespace CE

#if IS_MONOLITHIC

#else

#define IMPLEMENT_MODULE(ModuleClass)

#endif
