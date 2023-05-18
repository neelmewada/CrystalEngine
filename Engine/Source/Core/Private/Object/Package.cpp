
#include "CoreMinimal.h"



namespace CE
{
	Package::Package()
	{
		this->packageName = name;
	}

	Package::~Package()
	{

	}

	Package* Package::LoadPackage(const IO::Path& fullPackagePath, LoadFlags loadFlags)
	{
		LoadPackageResult result{};
		return LoadPackage(fullPackagePath, result, loadFlags);
	}

	Package* Package::LoadPackage(const IO::Path& fullPackagePath, LoadPackageResult& outResult, LoadFlags loadFlags)
	{
		if (!fullPackagePath.Exists())
		{
			outResult = LoadPackageResult::PackageNotFound;
			CE_LOG(Error, All, "Package not found at: {}", fullPackagePath);
			return nullptr;
		}
        
        

		return nullptr;
	}

	SavePackageResult Package::SavePackage(Package* package, Object* asset, const IO::Path& fullPackagePath, const SavePackageArgs& saveArgs)
	{
		if (package == nullptr)
		{
			CE_LOG(Error, All, "SavePackage() passed with nullptr package!");
			return SavePackageResult::UnknownError;
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

		FileStream stream = FileStream(fullPackagePath, Stream::Permissions::ReadWrite);

		return SavePackage(package, asset, &stream, saveArgs);
	}

	bool Package::ContainsObject(Object* object)
	{
		return ObjectPresentInHierarchy(object);
	}

} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, Package)

using namespace CE;
