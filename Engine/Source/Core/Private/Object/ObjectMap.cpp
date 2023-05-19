
#include "CoreMinimal.h"

namespace CE
{

	void ObjectMap::AddObject(Object* object)
	{
        if (object == nullptr || objects.KeyExists(object->GetUuid()))
            return;
        objects.Add({ object->GetUuid(), object });
	}

	void ObjectMap::RemoveObject(Object* object)
	{
		if (object == nullptr)
			return;
		
		objects.Remove(object->GetUuid());
	}

	void ObjectMap::RemoveObject(UUID uuid)
	{
		if (!objects.KeyExists(uuid))
			return;

		auto objectRef = objects[uuid];
		objects.Remove(uuid);
	}

	void ObjectMap::RemoveAll()
	{
		objects.Clear();
	}

} // namespace CE

CE_RTTI_POD_IMPL(CE, ObjectMap)

