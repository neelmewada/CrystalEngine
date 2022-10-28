#pragma once

#include "Misc/CoreDefines.h"
#include "Containers/String.h"
#include "Containers/HashMap.h"

#include "Module.h"

namespace CE
{
    typedef Module* (*LoadModuleFunc)();

    struct ModuleInfo
    {
        String ModuleName;
        Module* ModuleImpl;
        
        void* DllHandle;
        LoadModuleFunc LoadFuncPtr;

        bool bIsLoaded;
    };

    enum class ModuleLoadResult
    {
        Success = 0,
        AlreadyLoaded,
        DllNotFound,
        FailedToLoad,
        InvalidSymbols,
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

        Module* LoadModule(String moduleName, ModuleLoadResult& result);
        void UnloadModule(String moduleName);

    private:

        ModuleInfo* AddModule(String moduleName, ModuleLoadResult& result);

        ModuleInfo* FindModuleInfo(String moduleName);

        HashMap<String, ModuleInfo> ModuleMap{};
    };

} // namespace CE

#if IS_MONOLITHIC

#define IMPLEMENT_MODULE(ModuleClass)

#else

#define IMPLEMENT_MODULE(ModuleClass)\
CE::Module* LoadModule()\
{\
    return new ModuleClass();\
}\
void UnloadModule(CE::Module* modulePtr)\
{\
    delete modulePtr;\
}

#endif
