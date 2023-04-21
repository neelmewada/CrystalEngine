#pragma once

#include "CoreMinimal.h"

namespace CE
{
	class Asset;

	CLASS(Abstract)
	class SYSTEM_API ResourceObject : public Object
	{
		CE_CLASS(ResourceObject, CE::Object)
	protected:
		ResourceObject(Name name);
	public:
		virtual ~ResourceObject();

		static void RegisterAssetClassForResource(TypeId assetClassId, TypeId resourceClass);

		static ClassType* GetResourceClassFor(TypeId assetClassId);

		template<typename TAssetClass, typename TResourceClass>
		inline static void RegisterAssetClassForResource()
		{
			RegisterAssetClassForResource(TAssetClass::Type()->GetTypeId(), TResourceClass::Type()->GetTypeId());
		}

	private:
		static HashMap<TypeId, TypeId> assetClassIdToResourceClassMap;
	};

} // namespace CE

#include "ResourceObject.rtti.h"

#define CE_REGISTER_RESOURCE_CLASS(AssetClass, ResourceClass)
