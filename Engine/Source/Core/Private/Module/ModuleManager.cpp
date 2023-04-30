
#include "Module/ModuleManager.h"

#include "PAL/Common/PlatformProcess.h"

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
		TypeInfo::currentlyLoadingModule = moduleName;

		auto info = FindModuleInfo(moduleName);

		if (info != nullptr && info->isLoaded)
		{
			result = ModuleLoadResult::AlreadyLoaded;
			CoreDelegates::onModuleFailedToLoad.Broadcast(moduleName, result);

			TypeInfo::currentlyLoadingModule = NAME_None;
			return info->moduleImpl;
		}

		if (info == nullptr)
		{
			info = AddModule(moduleName, result);

			if (info == nullptr)
			{
				CoreDelegates::onModuleFailedToLoad.Broadcast(moduleName, result);

				TypeInfo::currentlyLoadingModule = NAME_None;
				return nullptr;
			}
		}

		// Load module
		Module* modulePtr = info->loadFuncPtr();
		if (modulePtr == nullptr)
		{
			result = ModuleLoadResult::InvalidModulePtr;
			CoreDelegates::onModuleFailedToLoad.Broadcast(moduleName, result);

			TypeInfo::currentlyLoadingModule = NAME_None;
			return nullptr;
		}

		CoreDelegates::onBeforeModuleLoad.Broadcast(info);

		info->isLoaded = true;
		info->moduleImpl = modulePtr;

		// Register types
		modulePtr->RegisterTypes();

		// Startup module
		modulePtr->StartupModule();

		CE_LOG(Info, All, "Loaded Module: {}", moduleName);

		CoreDelegates::onAfterModuleLoad.Broadcast(info);

		TypeInfo::currentlyLoadingModule = NAME_None;
		return modulePtr;
	}

	void ModuleManager::UnloadModule(String moduleName)
	{
		auto info = FindModuleInfo(moduleName);

		if (info == nullptr || !info->isLoaded || info->moduleImpl == nullptr)
		{
			return;
		}

		TypeInfo::currentlyUnloadingModule = moduleName;

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

		CE_LOG(Info, All, "Unloaded Module: {}", moduleName);

		TypeInfo::currentlyUnloadingModule = NAME_None;
	}

	Module* ModuleManager::LoadModule(String moduleName)
	{
		ModuleLoadResult result;
		return LoadModule(moduleName, result);
	}

	PluginModule* ModuleManager::LoadPluginModule(String moduleName, ModuleLoadResult& result)
	{
		TypeInfo::currentlyLoadingModule = moduleName;

		auto info = FindModuleInfo(moduleName);

		if (info != nullptr && info->isLoaded)
		{
			result = ModuleLoadResult::AlreadyLoaded;

			TypeInfo::currentlyLoadingModule = NAME_None;
			return (PluginModule*)info->moduleImpl;
		}

		if (info == nullptr)
		{
			info = AddPluginModule(moduleName, result);

			if (info == nullptr)
			{
				TypeInfo::currentlyLoadingModule = NAME_None;
				return nullptr;
			}
		}

		// Load module
		PluginModule* modulePtr = (PluginModule*)info->loadFuncPtr();
		if (modulePtr == nullptr)
		{
			result = ModuleLoadResult::InvalidModulePtr;

			TypeInfo::currentlyLoadingModule = NAME_None;
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

		TypeInfo::currentlyLoadingModule = NAME_None;
		return modulePtr;
	}

	void ModuleManager::UnloadPluginModule(String moduleName)
	{
		auto info = FindModuleInfo(moduleName);

		if (info == nullptr || !info->isLoaded || info->moduleImpl == nullptr)
		{
			return;
		}

		TypeInfo::currentlyUnloadingModule = moduleName;

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

		TypeInfo::currentlyUnloadingModule = NAME_None;
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

