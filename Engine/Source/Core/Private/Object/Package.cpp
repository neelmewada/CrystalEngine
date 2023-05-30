
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

	}

	IO::Path Package::GetPackagePath(const Name& packageName)
	{
		if (!packageName.IsValid())
			return {};
		String packageNameStr = packageName.GetString();

		if (packageNameStr.StartsWith("/Engine")) // Example: /Engine/Textures/Noise/Perlin/Atlas01.TexMap09
		{
			return PlatformDirectories::GetEngineRootDir() / packageNameStr.GetSubstring(1);
		}
		else if (packageNameStr.StartsWith("/Game"))
		{
			return PlatformDirectories::GetGameDir().GetParentPath();
		}

		return PlatformDirectories::GetAppRootDir() / packageNameStr.GetSubstring(1);
	}

	Package* Package::LoadPackage(Package* outer, Name packageName, LoadFlags loadFlags)
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
		if (!fullPackagePath.Exists())
		{
			outResult = LoadPackageResult::PackageNotFound;
			CE_LOG(Error, All, "Package not found at: {}", fullPackagePath);
			return nullptr;
		}
		if (fullPackagePath.IsDirectory())
		{
			outResult = LoadPackageResult::PackageNotFound;
			CE_LOG(Error, All, "Package path passed is a directory: {}", fullPackagePath);
			return nullptr;
		}
        
		FileStream stream = FileStream(fullPackagePath, Stream::Permissions::ReadOnly);

		return LoadPackage(package, &stream, fullPackagePath, outResult, loadFlags);
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

		return SavePackage(package, asset, packagePath, saveArgs);
	}

	SavePackageResult Package::SavePackage(Package* package, Object* asset, const IO::Path& fullPackagePath, const SavePackageArgs& saveArgs)
	{
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
		if (!fullPackagePath.Exists())
		{
			CE_LOG(Error, All, "SavePackage() passed with a package path that does not exist!");
			return SavePackageResult::InvalidPath;
		}
		if (fullPackagePath.IsDirectory())
		{
			CE_LOG(Error, All, "SavePackage() passed with a package path that is a directory!");
			return SavePackageResult::InvalidPath;
		}
		if (asset != nullptr && asset->GetPackage() != package)
		{
			CE_LOG(Error, All, "SavePackage() passed with an asset object that is not part of the package!");
			return SavePackageResult::AssetNotInPackage;
		}
        
        package->fullPackagePath = fullPackagePath;

		FileStream stream = FileStream(fullPackagePath, Stream::Permissions::ReadWrite);
		
		return SavePackage(package, asset, &stream, saveArgs);
	}

	bool Package::ContainsObject(Object* object)
	{
		return ObjectPresentInHierarchy(object);
	}

	Object* Package::ResolveObjectReference(UUID objectUuid)
	{
		if (!loadedSubobjects.KeyExists(objectUuid))
			return nullptr;
		return loadedSubobjects[objectUuid];
	}

} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, Package)


