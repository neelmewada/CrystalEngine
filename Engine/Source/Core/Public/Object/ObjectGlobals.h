#pragma once

#include "Package.h"

namespace CE
{
	namespace Internal
	{

		struct CORE_API ConstructObjectParams
		{
			ConstructObjectParams() = default;
			ConstructObjectParams(ClassType* objectClass)
				: objectClass(objectClass)
			{}

			Object* owner = nullptr;
			ClassType* objectClass = nullptr;
			Name name{};
			Object* templateObject = nullptr;
		};

		CORE_API Object* StaticConstructObject(const ConstructObjectParams& params);

		CORE_API void ConstructFromTemplate(Object* templateObject, Object* destObject);

	}

	template<typename TClass> requires std::is_base_of<Object, TClass>::value
	TClass* NewObject(Object* owner = (Object*)GetTransientPackage(), Name objectName = "", ClassType* objectClass = TClass::Type())
	{
		if (objectClass == nullptr || !objectClass->IsSubclassOf(TClass::Type()))
			return nullptr;

		Internal::ConstructObjectParams params{ objectClass };
		params.owner = owner;
		params.name = objectName;
		return static_cast<TClass*>(Internal::StaticConstructObject(params));
	}
    
}
