
#include "CoreMinimal.h"


namespace CE
{
	SharedMutex Package::packageRegistryMutex{};

	HashMap<Name, Package*> Package::loadedPackages{};
	HashMap<Uuid, Package*> Package::loadedPackagesByUuid{};
	HashMap<Uuid, Name> Package::loadedPackageUuidToPath{};

	Array<IPackageResolver*> Package::packageResolvers = {};

	Package::Package()
	{
		this->packageName = name;
	}

	Package::~Package()
	{
		LockGuard<SharedMutex> lock{ packageRegistryMutex };

        loadedPackages.Remove(packageName);
		loadedPackagesByUuid.Remove(GetUuid());
		loadedPackageUuidToPath.Remove(GetUuid());
	}

	Package* Package::LoadPackageByUuid(Uuid packageUuid, LoadFlags loadFlags)
	{
		ZoneScoped;

		LockGuard<SharedMutex> lock{ packageRegistryMutex };

		if (loadedPackagesByUuid.KeyExists(packageUuid))
			return loadedPackagesByUuid[packageUuid];
		if (packageResolvers.IsEmpty())
			return nullptr;

		for (int i = packageResolvers.GetSize() - 1; i >= 0; i--)
		{
			Name packagePath = packageResolvers[i]->GetPackagePath(packageUuid);
			if (!packagePath.IsValid())
				continue;
			Package* package = Package::LoadPackage(nullptr, packagePath, loadFlags);
			if (package != nullptr)
				return package;
		}

		return nullptr;
	}

	IO::Path Package::GetPackagePath(const Name& packageName)
	{
		if (!packageName.IsValid())
			return {};
		String packageNameStr = packageName.GetString();

		if (packageNameStr.StartsWith("/Engine/") || packageNameStr == "/Engine") // Example: /Engine/Assets/Textures/Noise/Perlin/Perlin04
		{
			if (!gProjectPath.Exists() || !(gProjectPath / "Engine/Assets").Exists())
			{
				return PlatformDirectories::GetEngineRootDir() / (packageNameStr.GetSubstring(1) + ".casset");
			}
			return gProjectPath / (packageNameStr.GetSubstring(1) + ".casset");
		}
		else if (packageNameStr.StartsWith("/Game/") || packageNameStr == "/Game")
		{
			return gProjectPath / (packageNameStr.GetSubstring(1) + ".casset");
		}
		else if (packageNameStr.StartsWith("/Editor/") || packageNameStr == "/Editor")
		{
			return PlatformDirectories::GetLaunchDir() / (packageNameStr.GetSubstring(1) + ".casset");
		}
		else if (packageNameStr.StartsWith("/Temp/") || packageNameStr == "/Temp")
		{
			return gProjectPath / (packageNameStr.GetSubstring(1) + ".temp");
		}

		return PlatformDirectories::GetAppRootDir() / (packageNameStr.GetSubstring(1) + ".casset");
	}

	bool Package::DestroyLoadedPackage(const Name& packageName)
	{
		LockGuard<SharedMutex> lock{ packageRegistryMutex };

		if (!loadedPackages.KeyExists(packageName))
			return false;
		loadedPackages[packageName]->Destroy(); // Destroying package automatically removes it from the HashMap.
		return true;
	}

	void Package::DestroyAllSubobjects()
	{
		for (int i = 0; i < GetSubObjectCount(); i++)
		{
			Object* subobject = GetSubobject(i);
			DetachSubobject(subobject);
			subobject->Destroy();
		}

		loadedObjects.Clear();
		objectUuidToEntryMap.Clear();
	}

	void Package::DestroyAllPackages()
	{
		LockGuard<SharedMutex> lock{ packageRegistryMutex };

		Array<Package*> packagesToDestroy{};
		for (auto& [packageName, package] : loadedPackages)
		{
			packagesToDestroy.Add(package);
		}
		for (auto package : packagesToDestroy)
		{
			package->Destroy();
		}
		packagesToDestroy.Clear();
		loadedPackages.Clear();
	}

	Package* Package::LoadPackage(Package* outer, const Name& packageName, LoadFlags loadFlags)
	{
		return LoadPackage(outer, GetPackagePath(packageName), loadFlags);
	}

	Package* Package::LoadPackage(Package* package, const IO::Path& fullPackagePath, LoadFlags loadFlags)
	{
		LoadPackageResult result{};
		return LoadPackage(package, fullPackagePath, result, loadFlags);
	}

	Package* Package::LoadPackage(Package* package, const IO::Path& fullPackagePath, LoadPackageResult& outResult, LoadFlags loadFlags)
	{
		String pathStr = fullPackagePath.GetString();
		ZoneScoped;
		ZoneTextF(pathStr.GetCString());

		auto path = fullPackagePath;
		if (path.GetExtension().IsEmpty()) // Add extension if one doesn't exist
		{
			path = path.GetString() + ".casset";
		}
		if (!path.Exists())
		{
			outResult = LoadPackageResult::PackageNotFound;
			//CE_LOG(Error, All, "Package not found at: {}", path);
			return nullptr;
		}
		if (path.IsDirectory())
		{
			outResult = LoadPackageResult::PackageNotFound;
			CE_LOG(Error, All, "Package path passed is a directory: {}", path);
			return nullptr;
		}
        
		FileStream stream = FileStream(path, Stream::Permissions::ReadOnly);
		stream.SetBinaryMode(true);
		return LoadPackage(package, &stream, path, outResult, loadFlags);
	}

	SavePackageResult Package::SavePackage(Package* package, Object* asset, const SavePackageArgs& saveArgs)
	{
		if (package == nullptr)
		{
			CE_LOG(Error, All, "SavePackage() passed with NULL package!");
			return SavePackageResult::InvalidPackage;
		}
		if (package->IsTransient())
		{
			CE_LOG(Error, All, "Could not save a transient package: {}", package->GetPackageName());
			return SavePackageResult::InvalidPackage;
		}

		IO::Path packagePath = GetPackagePath(package->GetPackageName());
		if (packagePath.GetExtension().IsEmpty())
		{
			packagePath = packagePath.GetString() + ".casset";
		}

		return SavePackage(package, asset, packagePath, saveArgs);
	}

	SavePackageResult Package::SavePackage(Package* package, Object* asset, const IO::Path& fullPackagePath, const SavePackageArgs& saveArgs)
	{
		auto path = fullPackagePath;
		if (path.GetExtension().IsEmpty())
		{
			path = path.GetString() + ".casset";
		}

		String pathStr = path.GetString();
		ZoneScoped;
		ZoneTextF(pathStr.GetCString());

		if (!path.GetParentPath().Exists())
		{
			IO::Path::CreateDirectories(path.GetParentPath());
		}

		if (package == nullptr)
		{
			CE_LOG(Error, All, "SavePackage() passed with NULL package!");
			return SavePackageResult::InvalidPackage;
		}
		if (package->IsTransient())
		{
			CE_LOG(Error, All, "SavePackage() passed with a transient package named: {}", package->GetPackageName());
			return SavePackageResult::InvalidPackage;
		}
		if (path.IsDirectory())
		{
			CE_LOG(Error, All, "SavePackage() passed with a package path that is a directory!");
			return SavePackageResult::InvalidPath;
		}
		if (asset != nullptr && asset->GetPackage() != package)
		{
			CE_LOG(Error, All, "SavePackage() passed with an asset object that is not part of the package!");
			return SavePackageResult::AssetNotInPackage;
		}
        
        package->fullPackagePath = path;

		FileStream stream = FileStream(path, Stream::Permissions::WriteOnly);
		
		return SavePackage(package, asset, &stream, saveArgs);
	}

	bool Package::ContainsObject(Object* object)
	{
		return IsObjectPresentInHierarchy(object);
	}

	Object* Package::ResolveObjectReference(Uuid objectUuid)
	{
		if (!loadedObjects.KeyExists(objectUuid))
			return nullptr;
		return loadedObjects[objectUuid];
	}

	void Package::OnSubobjectDetached(Object* subobject)
	{
		Super::OnSubobjectDetached(subobject);

		if (subobject->GetUuid() == primaryObjectUuid)
		{
			primaryObjectName = "";
			primaryObjectTypeName = "";
			primaryObjectUuid = 0;
		}
	}

	void Package::OnAfterDeserialize()
	{
		Super::OnAfterDeserialize();

		SetName(packageName);
	}

	const Name& Package::GetPrimaryObjectName()
	{
		GetPrimaryObjectUuid();

		if (!primaryObjectName.IsValid())
		{
			const auto& map = GetSubObjectMap();

			if (isFullyLoaded && GetSubObjectCount() > 0)
			{
				for (const auto& object : map)
				{
					if (object == nullptr)
						continue;

					if (object->IsOfType<Asset>())
					{
						primaryObjectName = object->GetName();
						return primaryObjectName;
					}

					primaryObjectName = object->GetName();
				}
			}
			else
			{
				for (const auto& [uuid, objectEntry] : objectUuidToEntryMap)
				{
					String pathInPackage = objectEntry.pathInPackage;
					if (pathInPackage.Contains(".") || pathInPackage.IsEmpty()) // object is not a root object in this package
						continue;

					ClassType* objectClass = ClassType::FindClass(objectEntry.objectClassName);
					if (objectClass == nullptr)
						continue;

					if (objectClass->IsSubclassOf<Asset>())
					{
						primaryObjectName = objectEntry.objectName;
						return primaryObjectName;
					}

					primaryObjectName = objectEntry.objectName;
				}
			}
		}
		return primaryObjectName;
	}

	const Name& Package::GetPrimaryObjectTypeName()
	{
		GetPrimaryObjectUuid();

		if (!primaryObjectTypeName.IsValid())
		{
			const auto& map = GetSubObjectMap();

			if (isFullyLoaded && GetSubObjectCount() > 0)
			{
				for (const auto& object : map)
				{
					if (object == nullptr)
						continue;

					if (object->IsOfType<Asset>())
					{
						primaryObjectTypeName = object->GetClass()->GetTypeName();
						return primaryObjectTypeName;
					}

					primaryObjectTypeName = object->GetClass()->GetTypeName();
				}
			}
			else
			{
				for (const auto& [uuid, objectEntry] : objectUuidToEntryMap)
				{
					String pathInPackage = objectEntry.pathInPackage;
					if (pathInPackage.Contains(".") || pathInPackage.IsEmpty()) // object is not a root object in this package
						continue;

					ClassType* objectClass = ClassType::FindClass(objectEntry.objectClassName);
					if (objectClass == nullptr)
						continue;

					if (objectClass->IsSubclassOf<Asset>())
					{
						primaryObjectTypeName = objectClass->GetTypeName();
						return primaryObjectTypeName;
					}

					primaryObjectTypeName = objectClass->GetTypeName();
				}
			}
		}
		return primaryObjectTypeName;
	}

	Uuid Package::GetPrimaryObjectUuid()
	{
		if (primaryObjectUuid == 0)
		{
			const auto& map = GetSubObjectMap();

			if (isFullyLoaded && GetSubObjectCount() > 0)
			{
				for (const auto& object : map)
				{
					if (object == nullptr)
						continue;

					if (object->IsOfType<Asset>())
					{
						primaryObjectUuid = object->GetUuid();
						return primaryObjectUuid;
					}

					primaryObjectUuid = object->GetUuid();
				}
			}
			else
			{
				for (const auto& [uuid, objectEntry] : objectUuidToEntryMap)
				{
					String pathInPackage = objectEntry.pathInPackage;
					if (pathInPackage.Contains(".") || pathInPackage.IsEmpty()) // object is not a root object in this package
						continue;

					ClassType* objectClass = ClassType::FindClass(objectEntry.objectClassName);
					if (objectClass == nullptr)
						continue;

					if (objectClass->IsSubclassOf<Asset>())
					{
						primaryObjectUuid = objectEntry.instanceUuid;
						return primaryObjectUuid;
					}

					primaryObjectUuid = objectEntry.instanceUuid;
				}
			}
		}
		return primaryObjectUuid;
	}

	String Package::GetPrimarySourceAssetRelativePath()
	{
#if PAL_TRAIT_BUILD_EDITOR
		const auto& map = GetSubObjectMap();

		if (isFullyLoaded && GetSubObjectCount() > 0)
		{
			for (const auto& object : map)
			{
				if (object != nullptr && object->IsOfType<Asset>())
					return ((Asset*)object)->GetSourceAssetRelativePath();
			}
		}
		else
		{
			for (const auto& [uuid, objectEntry] : objectUuidToEntryMap)
			{
				ClassType* objectClass = ClassType::FindClass(objectEntry.objectClassName);
				if (objectClass != nullptr && objectClass->IsSubclassOf<Asset>())
				{
					return objectEntry.sourceAssetRelativePath;
				}
			}
		}
#endif

		return String();
	}

	void Package::SetObjectUuid(Object* object, Uuid newUuid)
	{
		if (object == nullptr)
			return;

		loadedObjectsMutex.Lock();

		Uuid oldUuid = object->GetUuid();

		if (GetPrimaryObjectUuid() == oldUuid)
		{
			primaryObjectUuid = 0;
		}

		if (objectUuidToEntryMap.KeyExists(oldUuid))
		{
			objectUuidToEntryMap[newUuid] = objectUuidToEntryMap[oldUuid];
			objectUuidToEntryMap.Remove(oldUuid);

			objectUuidToEntryMap[newUuid].instanceUuid = newUuid;
		}

		if (loadedObjects.KeyExists(oldUuid))
		{
			loadedObjects[newUuid] = loadedObjects[oldUuid];
			loadedObjects.Remove(oldUuid);
		}

		loadedObjectsMutex.Unlock();

		packageRegistryMutex.Lock();

		if (object->IsPackage())
		{
			Package* package = (Package*)object;

			if (loadedPackagesByUuid.KeyExists(oldUuid))
			{
				loadedPackagesByUuid[newUuid] = loadedPackagesByUuid[oldUuid];
				loadedPackagesByUuid.Remove(oldUuid);
			}

			if (loadedPackageUuidToPath.KeyExists(oldUuid))
			{
				loadedPackageUuidToPath[newUuid] = loadedPackageUuidToPath[oldUuid];
				loadedPackageUuidToPath.Remove(oldUuid);
			}
		}

		packageRegistryMutex.Unlock();

		object->uuid = newUuid;
	}

	void Package::OnObjectUnloaded(Object* object)
	{
		if (object == nullptr)
			return;
		loadedObjects.Remove(object->GetUuid());
	}

} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, Package)


