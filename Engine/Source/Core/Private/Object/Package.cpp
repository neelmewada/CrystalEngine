
#include "CoreMinimal.h"


namespace CE
{
	HashMap<Name, Package*> Package::loadedPackages{};

	Package::Package()
	{
		this->packageName = name;
	}

	Package::~Package()
	{
        loadedPackages.Remove(packageName);
	}

	IO::Path Package::GetPackagePath(const PackagePath& packageName)
	{
		if (!packageName.IsValid())
			return {};
		String packageNameStr = packageName.GetString();

		if (packageNameStr.StartsWith("/Engine")) // Example: /Engine/Assets/Textures/Noise/Perlin/Perlin04
		{
			return PlatformDirectories::GetEngineRootDir() / packageNameStr.GetSubstring(1);
		}
		else if (packageNameStr.StartsWith("/Game"))
		{
			return PlatformDirectories::GetGameDir().GetParentPath() / packageNameStr.GetSubstring(1);
		}

		return PlatformDirectories::GetAppRootDir() / packageNameStr.GetSubstring(1);
	}

	Package* Package::LoadPackage(Package* outer, const PackagePath& packageName, LoadFlags loadFlags)
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
		auto path = fullPackagePath;
		if (path.GetExtension().IsEmpty()) // Add extension if one doesn't exist
		{
			path = path.GetString() + ".casset";
		}
		if (!path.Exists())
		{
			outResult = LoadPackageResult::PackageNotFound;
			CE_LOG(Error, All, "Package not found at: {}", path);
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

		FileStream stream = FileStream(path, Stream::Permissions::ReadWrite);
		
		return SavePackage(package, asset, &stream, saveArgs);
	}

	bool Package::ContainsObject(Object* object)
	{
		return ObjectPresentInHierarchy(object);
	}

	Object* Package::ResolveObjectReference(UUID objectUuid)
	{
		if (!loadedObjects.KeyExists(objectUuid))
			return nullptr;
		return loadedObjects[objectUuid];
	}

	void Package::OnSubobjectDetached(Object* subobject)
	{
		Super::OnSubobjectDetached(subobject);

		if (subobject->GetName() == primaryObjectName)
		{
			primaryObjectName = "";
			primaryObjectTypeName = "";
		}
	}

	const Name& Package::GetPrimaryObjectName()
	{
		if (!primaryObjectName.IsValid())
		{
			const auto& map = GetSubObjectMap();

			if (isFullyLoaded && GetSubObjectCount() > 0)
			{
				for (const auto& [uuid, object] : map)
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
		if (!primaryObjectTypeName.IsValid())
		{
			const auto& map = GetSubObjectMap();

			if (isFullyLoaded && GetSubObjectCount() > 0)
			{
				for (const auto& [uuid, object] : map)
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

	void Package::OnObjectUnloaded(Object* object)
	{
		if (object == nullptr)
			return;
		loadedObjects.Remove(object->GetUuid());
	}

} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, Package)


