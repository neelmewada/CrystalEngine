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

		HashMap<Uuid, Object*> objectsToSerialize{};
		objectsToSerialize.Add({ asset->GetUuid(), asset });
		asset->FetchObjectReferences(objectsToSerialize);
		Array<Name> packageDependencies{};

		for (const auto& [objectUuid, objectInstance] : objectsToSerialize) //Find external package dependencies
		{
			if (objectInstance == nullptr || objectUuid == 0)
				continue;
			if (objectInstance->IsTransient())
				continue;

			auto objectPackage = objectInstance->GetPackage();

			if (package != objectPackage && objectPackage != nullptr)
			{
				packageDependencies.Add(objectPackage->GetPackageName());
			}
		}

		

		return SavePackageResult::Success;
	}
    
} // namespace CE
