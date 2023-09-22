#include "CoreMinimal.h"

#include "../Package.inl"

#include "ryml.hpp"

namespace CE
{

	SavePackageResult Package::SavePackage2(Package* package, Object* asset, Stream* stream, const SavePackageArgs& saveArgs)
	{
		if (package == nullptr)
		{
			return SavePackageResult::InvalidPackage;
		}
		if (package->IsTransient())
		{
			CE_LOG(Error, All, "Could not save a transient package: {}", package->GetPackageName());
			return SavePackageResult::InvalidPackage;
		}
		if (stream == nullptr)
		{
			return SavePackageResult::UnknownError;
		}
		if (asset == nullptr)
			asset = package;

		HashMap<UUID, Object*> objectsToSerialize{};
		objectsToSerialize.Add({ asset->GetUuid(), asset });
		asset->FetchObjectReferences(objectsToSerialize);
		Array<Name> packageDependencies{};

		ryml::Tree tree{};
		ryml::NodeRef root = tree.rootref();
		

		return SavePackageResult::Success;
	}
    
} // namespace CE
