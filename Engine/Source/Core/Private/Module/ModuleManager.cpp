
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
		auto info = FindModuleInfo(moduleName);

		if (info != nullptr && info->isLoaded)
		{
			result = ModuleLoadResult::AlreadyLoaded;
			return info->moduleImpl;
		}

		if (info == nullptr)
		{
			info = AddModule(moduleName, result);

			if (info == nullptr)
			{
				return nullptr;
			}
		}

		// Load module
		Module* modulePtr = info->loadFuncPtr();
		if (modulePtr == nullptr)
		{
			result = ModuleLoadResult::InvalidModulePtr;
			return nullptr;
		}

		info->isLoaded = true;
		info->moduleImpl = modulePtr;

		// Register types
		modulePtr->RegisterTypes();

		// Startup module
		modulePtr->StartupModule();

		CE_LOG(Info, All, "Loaded Module: {}", moduleName);

		return modulePtr;
	}

	void ModuleManager::UnloadModule(String moduleName)
	{
		auto info = FindModuleInfo(moduleName);

		if (info == nullptr || !info->isLoaded || info->moduleImpl == nullptr)
		{
			return;
		}

		// Shutdown module
		info->moduleImpl->ShutdownModule();

		// Unload module
		info->isLoaded = false;
		info->unloadFuncPtr(info->moduleImpl);
		PlatformProcess::UnloadDll(info->dllHandle);

		// Remove module
		ModuleMap.Remove(moduleName);

		CE_LOG(Info, All, "Unloaded Module: {}", moduleName);
	}

	Module* ModuleManager::LoadModule(String moduleName)
	{
		ModuleLoadResult result;
		return LoadModule(moduleName, result);
	}

	PluginModule* ModuleManager::LoadPluginModule(String moduleName, ModuleLoadResult& result)
	{
		auto info = FindModuleInfo(moduleName);

		if (info != nullptr && info->isLoaded)
		{
			result = ModuleLoadResult::AlreadyLoaded;
			return (PluginModule*)info->moduleImpl;
		}

		if (info == nullptr)
		{
			info = AddPluginModule(moduleName, result);

			if (info == nullptr)
			{
				return nullptr;
			}
		}

		// Load module
		PluginModule* modulePtr = (PluginModule*)info->loadFuncPtr();
		if (modulePtr == nullptr)
		{
			result = ModuleLoadResult::InvalidModulePtr;
			return nullptr;
		}

		info->isLoaded = true;
		info->moduleImpl = modulePtr;

		// Register types
		modulePtr->RegisterTypes();

		// Startup module
		modulePtr->StartupModule();

		CE_LOG(Info, All, "Loaded Plugin: {}", moduleName);

		return modulePtr;
	}

	void ModuleManager::UnloadPluginModule(String moduleName)
	{
		auto info = FindModuleInfo(moduleName);

		if (info == nullptr || !info->isLoaded || info->moduleImpl == nullptr)
		{
			return;
		}

		// Shutdown module
		info->moduleImpl->ShutdownModule();

		// Unload module
		info->isLoaded = false;
		info->unloadFuncPtr(info->moduleImpl);
		PlatformProcess::UnloadDll(info->dllHandle);

		// Remove module
		ModuleMap.Remove(moduleName);

		CE_LOG(Info, All, "Unloaded Plugin: {}", moduleName);
	}

	PluginModule* ModuleManager::LoadPluginModule(String moduleName)
	{
		ModuleLoadResult result;
		return LoadPluginModule(moduleName, result);
	}

	String ModuleManager::GetLoadedModuleName(Module* modulePtr)
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

		return "";
	}

	ModuleInfo* ModuleManager::AddModule(String moduleName, ModuleLoadResult& result)
	{
		IO::Path moduleDllPath = PlatformProcess::GetModuleDllPath(moduleName);
		if (moduleDllPath.IsEmpty())
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

		auto loadFunction = (LoadModuleFunc)PlatformProcess::GetDllSymbol(dllHandle, "LoadModule");
		if (loadFunction == nullptr)
		{
			result = ModuleLoadResult::InvalidSymbols;
			return nullptr;
		}

		auto unloadFuntion = (UnloadModuleFunc)PlatformProcess::GetDllSymbol(dllHandle, "UnloadModule");
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
		IO::Path enginePluginDir = PlatformDirectories::GetEngineDir() / "Plugins";
		IO::Path editorPluginDir = PlatformDirectories::GetEditorDir() / "Plugins";

		//IO::Path engineModulePath = enginePluginDir / moduleName / (PlatformProcess::GetDllDecoratedName(moduleName));
		//IO::Path editorModulePath = editorPluginDir / moduleName / (PlatformProcess::GetDllDecoratedName(moduleName));

		String dllName = PlatformProcess::GetDllDecoratedName(moduleName);
		IO::Path pluginDllPath = PlatformDirectories::GetLaunchDir() / dllName;

		if (!pluginDllPath.Exists())
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

		auto loadFunction = (LoadModuleFunc)PlatformProcess::GetDllSymbol(dllHandle, "LoadModule");
		if (loadFunction == nullptr)
		{
			result = ModuleLoadResult::InvalidSymbols;
			return nullptr;
		}

		auto unloadFuntion = (UnloadModuleFunc)PlatformProcess::GetDllSymbol(dllHandle, "UnloadModule");
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

