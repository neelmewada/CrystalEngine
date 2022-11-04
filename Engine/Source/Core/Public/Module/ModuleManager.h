#pragma once

#include "Misc/CoreDefines.h"
#include "Containers/String.h"
#include "Containers/HashMap.h"

#include "Module.h"

namespace CE
{
    typedef Module* (*LoadModuleFunc)();
    typedef void (*UnloadModuleFunc)(Module* modulePtr);

    struct ModuleInfo
    {
        String ModuleName;
        
        void* DllHandle;
        LoadModuleFunc LoadFuncPtr;
        UnloadModuleFunc UnloadFuncPtr;

        bool bIsLoaded;
        Module* ModuleImpl; // nullptr if not loaded
    };

    enum class ModuleLoadResult
    {
        Success = 0,
        AlreadyLoaded,
        DllNotFound,
        FailedToLoad,
        InvalidSymbols,
        InvalidModulePtr,
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

        Module* LoadModule(String moduleName);

    private:

        ModuleInfo* AddModule(String moduleName, ModuleLoadResult& result);

        ModuleInfo* FindModuleInfo(String moduleName);

        HashMap<String, ModuleInfo> ModuleMap{};
    };

} // namespace CE

#if IS_MONOLITHIC

#define CE_IMPLEMENT_MODULE(ModuleName, ModuleImplClass)

#else

#define CE_IMPLEMENT_MODULE(ModuleName, ModuleImplClass)\
extern "C" DLL_EXPORT CE::Module* LoadModule()\
{\
    return new ModuleImplClass();\
}\
extern "C" DLL_EXPORT void UnloadModule(CE::Module* modulePtr)\
{\
    delete modulePtr;\
}

#endif
