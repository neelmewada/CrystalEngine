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
        Name moduleName;
        
        void* dllHandle;
        LoadModuleFunc loadFuncPtr;
        UnloadModuleFunc unloadFuncPtr;

        bool isLoaded;
        bool isPlugin = false;
        Module* moduleImpl; // nullptr if not loaded
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

    enum class PluginLoadType
    {
        DontLoad,
        LoadEarliest,
        LoadOnPreInit,
        LoadOnInit,
        LoadOnPostInit
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

        PluginModule* LoadPluginModule(String moduleName, ModuleLoadResult& result);
        void UnloadPluginModule(String moduleName);

        PluginModule* LoadPluginModule(String moduleName);

        Name GetLoadedModuleName(Module* modulePtr);

    private:

        ModuleInfo* AddModule(String moduleName, ModuleLoadResult& result);
        ModuleInfo* AddPluginModule(String moduleName, ModuleLoadResult& result);

        ModuleInfo* FindModuleInfo(String moduleName);

        HashMap<Name, ModuleInfo> ModuleMap{};
    };

} // namespace CE

#if PAL_TRAIT_BUILD_MONOLITHIC

// TODO: Monolithic implementation
#define CE_IMPLEMENT_MODULE(ModuleName, ModuleImplClass)

#else

#define CE_IMPLEMENT_MODULE(ModuleName, ModuleImplClass)\
extern "C" DLL_EXPORT CE::Module* CELoadModule()\
{\
    return new ModuleImplClass();\
}\
extern "C" DLL_EXPORT void CEUnloadModule(CE::Module* modulePtr)\
{\
    delete modulePtr;\
}

#define CE_IMPLEMENT_MODULE_AUTORTTI(ModuleName, ModuleImplClass)\
extern "C" DLL_EXPORT CE::Module* CELoadModule()\
{\
    CERegisterModuleTypes();\
    return new ModuleImplClass();\
}\
extern "C" DLL_EXPORT void CEUnloadModule(CE::Module* modulePtr)\
{\
    delete modulePtr;\
}

#define CE_IMPLEMENT_PLUGIN(PluginName, PluginImplClass)\
extern "C" DLL_EXPORT CE::Module* CELoadModule()\
{\
    return new PluginImplClass();\
}\
extern "C" DLL_EXPORT void CEUnloadModule(CE::Module* modulePtr)\
{\
    delete modulePtr;\
}

#endif
