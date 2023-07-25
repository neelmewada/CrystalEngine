#pragma once

#include "Misc/CoreDefines.h"
#include "Containers/String.h"
#include "Containers/HashMap.h"

#include "Module.h"

namespace CE
{
    typedef Module* (*LoadModuleFunc)();
    typedef void (*UnloadModuleFunc)(Module* modulePtr);

	typedef void (*LoadResourcedFunc)();
	typedef void (*UnloadResourcedFunc)();

	class Package;

    struct ModuleInfo
    {
        Name moduleName;
        
        void* dllHandle;
        LoadModuleFunc loadFuncPtr;
        UnloadModuleFunc unloadFuncPtr;

		LoadResourcedFunc loadResourcesFuncPtr;
		UnloadResourcedFunc unloadResourcesFuncPtr;

        bool isLoaded;
        bool isPlugin = false;
        Module* moduleImpl; // nullptr if not loaded

		Package* transientPackage = nullptr;
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
        
        bool IsModuleLoaded(String moduleName);

		Module* GetLoadedModule(const String& moduleName);

		/// Returns the transient package of a loaded module. Returns nullptr if module is not loaded or not found.
		Package* GetLoadedModuleTransientPackage(const String& moduleName);

    private:

        ModuleInfo* AddModule(String moduleName, ModuleLoadResult& result);
        ModuleInfo* AddPluginModule(String moduleName, ModuleLoadResult& result);

        ModuleInfo* FindModuleInfo(String moduleName);

        HashMap<Name, ModuleInfo> ModuleMap{};
    };

} // namespace CE

#define __CE_AUTORTTI_REGISTER__AUTORTTI()
#define __CE_AUTORTTI_REGISTER_0()
#define __CE_AUTORTTI_REGISTER_1() CERegisterModuleTypes()

#define __CE_RESOURCES_REGISTER__RESOURCES()
#define __CE_RESOURCES_REGISTER_0()
#define __CE_RESOURCES_REGISTER_1() CERegisterModuleResources()

#define __CE_RESOURCES_DEREGISTER__RESOURCES()
#define __CE_RESOURCES_DEREGISTER_0()
#define __CE_RESOURCES_DEREGISTER_1() CEDeregisterModuleResources()

#if PAL_TRAIT_BUILD_MONOLITHIC

// TODO: Monolithic implementation
#define CE_IMPLEMENT_MODULE(ModuleName, ModuleImplClass)

#else

#define CE_IMPLEMENT_MODULE(ModuleName, ModuleImplClass)\
extern "C" DLL_EXPORT CE::Module* CELoadModule()\
{\
    CE_EXPAND(CE_CONCATENATE(__CE_AUTORTTI_REGISTER_, _AUTORTTI))();\
    return new ModuleImplClass();\
}\
extern "C" DLL_EXPORT void CEUnloadModule(CE::Module* modulePtr)\
{\
    delete modulePtr;\
}\
extern "C" DLL_EXPORT void CELoadResources()\
{\
	CE_EXPAND(CE_CONCATENATE(__CE_RESOURCES_REGISTER_, _RESOURCES))();\
}\
extern "C" DLL_EXPORT void CEUnloadResources()\
{\
	CE_EXPAND(CE_CONCATENATE(__CE_RESOURCES_DEREGISTER_, _RESOURCES))();\
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


#endif
