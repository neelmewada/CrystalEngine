
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

		if (info != nullptr && info->bIsLoaded)
		{
			result = ModuleLoadResult::AlreadyLoaded;
			return info->ModuleImpl;
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
		Module* modulePtr = info->LoadFuncPtr();
		if (modulePtr == nullptr)
		{
			result = ModuleLoadResult::InvalidModulePtr;
			return nullptr;
		}

		info->bIsLoaded = true;
		info->ModuleImpl = modulePtr;

		// Register types
		modulePtr->RegisterTypes();

		// Startup module
		modulePtr->StartupModule();

		CE_LOG(Info, All, "Loaded Module: {}", moduleName);

		return nullptr;
	}

	void ModuleManager::UnloadModule(String moduleName)
	{
		auto info = FindModuleInfo(moduleName);

		if (info == nullptr || !info->bIsLoaded || info->ModuleImpl == nullptr)
		{
			return;
		}

		// Shutdown module
		info->ModuleImpl->ShutdownModule();

		// Unload module
		info->bIsLoaded = false;
		info->UnloadFuncPtr(info->ModuleImpl);
		PlatformProcess::UnloadDll(info->DllHandle);

		// Remove module
		ModuleMap.Remove(moduleName);

		CE_LOG(Info, All, "Unloaded Module: {}", moduleName);
	}

	Module* ModuleManager::LoadModule(String moduleName)
	{
		ModuleLoadResult result;
		return LoadModule(moduleName, result);
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
		ptr->bIsLoaded = false;
		ptr->DllHandle = dllHandle;
		ptr->LoadFuncPtr = loadFunction;
		ptr->UnloadFuncPtr = unloadFuntion;
		ptr->ModuleName = moduleName;
		ptr->ModuleImpl = nullptr;

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

