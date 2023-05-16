
#include "CoreMinimal.h"

namespace CE
{
	Package::Package()
	{
        
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

} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, Package)

using namespace CE;
