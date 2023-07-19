
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

		// Register types
		modulePtr->RegisterTypes();

		// Create transient package
		if (moduleName != "Core")
			info->transientPackage = CreateObject<Package>(nullptr, "/" + moduleName + "/Transient", OF_Transient);

		// Startup module
		modulePtr->StartupModule();

		if (moduleName == "Core")
			info->transientPackage = CreateObject<Package>(nullptr, "/" + moduleName + "/Transient", OF_Transient);

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

		// Shutdown module
		info->moduleImpl->ShutdownModule();

		// Delete transient package
		info->transientPackage->RequestDestroy();
		info->transientPackage = nullptr;

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

	PluginModule* ModuleManager::LoadPluginModule(String moduleName, ModuleLoadResult& result)
	{
		TypeInfo::currentlyLoadingModuleStack.Push(moduleName);

		auto info = FindModuleInfo(moduleName);

		if (info != nullptr && info->isLoaded)
		{
			result = ModuleLoadResult::AlreadyLoaded;

			TypeInfo::currentlyLoadingModuleStack.Pop();
			return (PluginModule*)info->moduleImpl;
		}

		if (info == nullptr)
		{
			info = AddPluginModule(moduleName, result);

			if (info == nullptr)
			{
				TypeInfo::currentlyLoadingModuleStack.Pop();
				return nullptr;
			}
		}

		// Load module
		PluginModule* modulePtr = (PluginModule*)info->loadFuncPtr();
		if (modulePtr == nullptr)
		{
			result = ModuleLoadResult::InvalidModulePtr;

			TypeInfo::currentlyLoadingModuleStack.Pop();
			return nullptr;
		}

		CoreDelegates::onBeforeModuleLoad.Broadcast(info);

		info->isLoaded = true;
		info->moduleImpl = modulePtr;

		// Register types
		modulePtr->RegisterTypes();

		// Startup module
		modulePtr->StartupModule();

		CE_LOG(Info, All, "Loaded Plugin: {}", moduleName);

		CoreDelegates::onAfterModuleLoad.Broadcast(info);

		TypeInfo::currentlyLoadingModuleStack.Pop();
		return modulePtr;
	}

	void ModuleManager::UnloadPluginModule(String moduleName)
	{
		auto info = FindModuleInfo(moduleName);

		if (info == nullptr || !info->isLoaded || info->moduleImpl == nullptr)
		{
			return;
		}

		TypeInfo::currentlyUnloadingModuleStack.Push(moduleName);

		CoreDelegates::onBeforeModuleUnload.Broadcast(info);

		// Shutdown module
		info->moduleImpl->ShutdownModule();

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

		CE_LOG(Info, All, "Unloaded Plugin: {}", moduleName);

		TypeInfo::currentlyUnloadingModuleStack.Pop();
	}

	PluginModule* ModuleManager::LoadPluginModule(String moduleName)
	{
		ModuleLoadResult result;
		return LoadPluginModule(moduleName, result);
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

		ModuleMap.Emplace(moduleName, ModuleInfo{});
		
		ModuleInfo* ptr = &ModuleMap[moduleName];
		ptr->isLoaded = false;
		ptr->isPlugin = false;
		ptr->dllHandle = dllHandle;
		ptr->loadFuncPtr = loadFunction;
		ptr->unloadFuncPtr = unloadFuntion;
		ptr->moduleName = moduleName;
		ptr->moduleImpl = nullptr;

		result = ModuleLoadResult::Success;

		return ptr;
	}

	ModuleInfo* ModuleManager::AddPluginModule(String moduleName, ModuleLoadResult& result)
	{
		String dllName = PlatformProcess::GetDllDecoratedName(moduleName);
		IO::Path pluginDllPath = PlatformDirectories::GetLaunchDir() / dllName;

		if (!pluginDllPath.Exists() || !pluginDllPath.Exists())
		{
			result = ModuleLoadResult::DllNotFound;
			return nullptr;
		}

		void* dllHandle = PlatformProcess::LoadDll(pluginDllPath);
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

		ModuleMap.Emplace(moduleName, ModuleInfo{});

		ModuleInfo* ptr = &ModuleMap[moduleName];
		ptr->isLoaded = false;
		ptr->isPlugin = true;
		ptr->dllHandle = dllHandle;
		ptr->loadFuncPtr = loadFunction;
		ptr->unloadFuncPtr = unloadFuntion;
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

