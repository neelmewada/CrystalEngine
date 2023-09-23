#include "CoreMinimal.h"

#include "../Package.inl"


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

		for (const auto& [objectUuid, objectInstance] : objectsToSerialize) // External package dependencies
		{
			if (objectInstance == nullptr || objectUuid == 0)
				continue;
			if (objectInstance->IsTransient())
				continue;

			if (package != objectInstance->GetPackage() && objectInstance->GetPackage() != nullptr)
			{
				packageDependencies.Add(objectInstance->GetPackage()->GetPackageName());
			}
		}



		return SavePackageResult::Success;
	}
    
} // namespace CE
