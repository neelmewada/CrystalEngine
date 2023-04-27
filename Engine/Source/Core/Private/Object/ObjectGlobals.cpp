
#include "CoreMinimal.h"

namespace CE
{
	namespace Internal
	{

		CORE_API Object* StaticConstructObject(const ConstructObjectParams& params)
		{
			if (params.objectClass == nullptr || !params.objectClass->CanBeInstantiated() || !params.objectClass->IsObject())
				return nullptr;

			auto instance = (Object*)params.objectClass->CreateInstance();
			if (instance == nullptr)
				return nullptr;

			if (params.templateObject != nullptr)
			{
				ConstructFromTemplate(params.templateObject, instance);
			}

			Name name = params.name;
			if (!name.IsValid())
				name = params.objectClass->GenerateInstanceName(instance->GetUuid());
			instance->SetName(name);

			if (params.owner != nullptr)
				params.owner->AttachSubobject(instance);
			return instance;
		}

		CORE_API void ConstructFromTemplate(Object* templateObject, Object* destObject)
		{
			if (templateObject == nullptr || destObject == nullptr)
				return;

			auto templateClass = (ClassType*)templateObject->GetType();
			auto destClass = (ClassType*)destObject->GetType();

			if (!destClass->IsSubclassOf(templateClass) && !templateClass->IsSubclassOf(destClass))
				return;

			// TODO: Loading from template object
		}

	}
    
} // namespace CE
