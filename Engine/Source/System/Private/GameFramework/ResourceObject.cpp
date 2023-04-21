
#include "System.h"

namespace CE
{

	HashMap<TypeId, TypeId> ResourceObject::assetClassIdToResourceClassMap{};

	ResourceObject::ResourceObject(Name name)
		: Object(name)
	{

	}

	ResourceObject::~ResourceObject()
	{

	}

	void ResourceObject::RegisterAssetClassForResource(TypeId assetClassId, TypeId resourceClass)
	{
		assetClassIdToResourceClassMap[assetClassId] = resourceClass;
	}

	ClassType* ResourceObject::GetResourceClassFor(TypeId assetClassId)
	{
		if (!assetClassIdToResourceClassMap.KeyExists(assetClassId))
			return nullptr;
		auto type = GetTypeInfo(assetClassIdToResourceClassMap[assetClassId]);
		if (type == nullptr || !type->IsClass() || !type->IsAssignableTo(TYPEID(ResourceObject)))
			return nullptr;
		return (ClassType*)type;
	}

} // namespace CE

