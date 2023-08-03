
#include "CoreMinimal.h"

namespace CE
{

	ModuleManager::ModuleManager()
	{

	}

	ModuleManager::~ModuleManager()
	{

	}

	Module* ModuleManager::LoadModule(String moduleName, ModuleLoadResult& result)
	{
		TypeInfo::currentlyLoadingModuleStack.Push(moduleName);
		auto info = FindModuleInfo(moduleName);

		if (info != nullptr && info->isLoaded)
		{
			result = ModuleLoadResult::AlreadyLoaded;
			CoreDelegates::onModuleFailedToLoad.Broadcast(moduleName, result);

			TypeInfo::currentlyLoadingModuleStack.Pop();
			return info->moduleImpl;
		}

		if (info == nullptr)
		{
			info = AddModule(moduleName, result);

			if (info == nullptr)
			{
				CoreDelegates::onModuleFailedToLoad.Broadcast(moduleName, result);

				TypeInfo::currentlyLoadingModuleStack.Pop();
				return nullptr;
			}
		}

		// Load module
		Module* modulePtr = info->loadFuncPtr();
		if (modulePtr == nullptr)
		{
			result = ModuleLoadResult::InvalidModulePtr;
			CoreDelegates::onModuleFailedToLoad.Broadcast(moduleName, result);

			TypeInfo::currentlyLoadingModuleStack.Pop();
			return nullptr;
		}

		CoreDelegates::onBeforeModuleLoad.Broadcast(info);

		info->isLoaded = true;
		info->moduleImpl = modulePtr;

		// Create transient package for if NOT Core module
		if (moduleName != "Core")
			info->transientPackage = CreateObject<Package>(nullptr, "/" + moduleName + "/Transient", OF_Transient);

		// Register custom types
		modulePtr->RegisterTypes();

		// Register AutoRTTI types
		info->loadTypesFuncPtr();

		// Startup module
		modulePtr->StartupModule();

		if (moduleName == "Core")
			info->transientPackage = CreateObject<Package>(nullptr, "/" + moduleName + "/Transient", OF_Transient);

		// Load resources
		info->loadResourcesFuncPtr();

		CE_LOG(Info, All, "Loaded Module: {}", moduleName);

		CoreDelegates::onAfterModuleLoad.Broadcast(info);

		TypeInfo::currentlyLoadingModuleStack.Pop();
		return modulePtr;
	}

	void ModuleManager::UnloadModule(String moduleName)
	{
		auto info = FindModuleInfo(moduleName);

		if (info == nullptr || !info->isLoaded || info->moduleImpl == nullptr)
		{
			return;
		}

		TypeInfo::currentlyUnloadingModuleStack.Push(moduleName);

		CoreDelegates::onBeforeModuleUnload.Broadcast(info);

		// Unload resources
		info->unloadResourcesFuncPtr();

		// Shutdown module
		info->moduleImpl->ShutdownModule();

		// Delete transient package
		if (info->transientPackage != nullptr)
		{
			info->transientPackage->RequestDestroy();
			info->transientPackage = nullptr;
		}

		// Deregister types
		info->moduleImpl->DeregisterTypes();

		// Unload module
		info->isLoaded = false;
		info->unloadFuncPtr(info->moduleImpl);

		CoreDelegates::onAfterModuleUnload.Broadcast(info);

		PlatformProcess::UnloadDll(info->dllHandle);
		info->dllHandle = nullptr;

		// Remove module
		ModuleMap.Remove(moduleName);

		CE_LOG(Info, All, "Unloaded Module: {}", moduleName);

		TypeInfo::currentlyUnloadingModuleStack.Pop();
	}

	Module* ModuleManager::LoadModule(String moduleName)
	{
		ModuleLoadResult result;
		return LoadModule(moduleName, result);
	}

	Name ModuleManager::GetLoadedModuleName(Module* modulePtr)
	{
		if (modulePtr == nullptr)
			return "";

		for (auto [moduleName, moduleInfo] : ModuleMap)
		{
			if (moduleInfo.moduleImpl == modulePtr)
			{
				return moduleName;
			}
		}

		return NAME_None;
	}

    bool ModuleManager::IsModuleLoaded(String moduleName)
    {
        auto info = FindModuleInfo(moduleName);
        if (info == nullptr)
            return false;
        
        return info->isLoaded;
    }

	Module* ModuleManager::GetLoadedModule(const String& moduleName)
	{
		auto info = FindModuleInfo(moduleName);
		if (info == nullptr || !info->isLoaded)
			return nullptr;

		return info->moduleImpl;
	}

	Package* ModuleManager::GetLoadedModuleTransientPackage(const String& moduleName)
	{
		ModuleInfo* info = FindModuleInfo(moduleName);
		if (info == nullptr || !info->isLoaded)
			return nullptr;
		
		return info->transientPackage;
	}

	ModuleInfo* ModuleManager::AddModule(String moduleName, ModuleLoadResult& result)
	{
		IO::Path moduleDllPath = PlatformProcess::GetModuleDllPath(moduleName);
		if (moduleDllPath.IsEmpty() || !moduleDllPath.Exists())
		{
			result = ModuleLoadResult::DllNotFound;
			return nullptr;
		}

		void* dllHandle = PlatformProcess::LoadDll(moduleDllPath);
		if (dllHandle == nullptr)
		{
			result = ModuleLoadResult::FailedToLoad;
			return nullptr;
		}

		auto loadFunction = (LoadModuleFunc)PlatformProcess::GetDllSymbol(dllHandle, "CELoadModule");
		if (loadFunction == nullptr)
		{
			result = ModuleLoadResult::InvalidSymbols;
			return nullptr;
		}

		auto unloadFuntion = (UnloadModuleFunc)PlatformProcess::GetDllSymbol(dllHandle, "CEUnloadModule");
		if (unloadFuntion == nullptr)
		{
			result = ModuleLoadResult::InvalidSymbols;
			return nullptr;
		}

		auto loadResourcesFuncPtr = (LoadResourcedFunc)PlatformProcess::GetDllSymbol(dllHandle, "CELoadResources");
		if (loadResourcesFuncPtr == nullptr)
		{
			result = ModuleLoadResult::InvalidSymbols;
			return nullptr;
		}

		auto unloadResourcesFuncPtr = (UnloadResourcedFunc)PlatformProcess::GetDllSymbol(dllHandle, "CEUnloadResources");
		if (unloadResourcesFuncPtr == nullptr)
		{
			result = ModuleLoadResult::InvalidSymbols;
			return nullptr;
		}

		auto loadTypesFuncPtr = (LoadTypesFunc)PlatformProcess::GetDllSymbol(dllHandle, "CELoadTypes");
		if (loadTypesFuncPtr == nullptr)
		{
			result = ModuleLoadResult::InvalidSymbols;
			return nullptr;
		}

		ModuleMap.Emplace(moduleName, ModuleInfo{});
		
		ModuleInfo* ptr = &ModuleMap[moduleName];
		ptr->isLoaded = false;
		ptr->isPlugin = false;
		ptr->dllHandle = dllHandle;
		ptr->loadFuncPtr = loadFunction;
		ptr->unloadFuncPtr = unloadFuntion;
		ptr->loadResourcesFuncPtr = loadResourcesFuncPtr;
		ptr->unloadResourcesFuncPtr = unloadResourcesFuncPtr;
		ptr->loadTypesFuncPtr = loadTypesFuncPtr;
		ptr->moduleName = moduleName;
		ptr->moduleImpl = nullptr;

		result = ModuleLoadResult::Success;

		return ptr;
	}

	ModuleInfo* ModuleManager::FindModuleInfo(String moduleName)
	{
		if (!ModuleMap.KeyExists(moduleName))
			return nullptr;
		return &ModuleMap[moduleName];
	}
}

