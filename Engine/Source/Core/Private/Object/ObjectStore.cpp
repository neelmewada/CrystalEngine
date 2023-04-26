
#include "CoreMinimal.h"

namespace CE
{

	void ObjectStore::AddObject(Object* object)
	{
        if (object == nullptr || objects.KeyExists(object->GetUuid()))
            return;
        objects.Add({ object->GetUuid(), object });
	}

	void ObjectStore::RemoveObject(Object* object)
	{
		if (object == nullptr)
			return;

		objects.Remove(object->GetUuid());
	}

	void ObjectStore::RemoveObjectWithUuid(UUID uuid)
	{
		if (!objects.KeyExists(uuid))
			return;

		auto objectRef = objects[uuid];
		objects.Remove(uuid);
	}

	void ObjectStore::DestroyAll()
	{
		for (auto [uuid, object] : objects)
		{
			delete object;
		}
		Clear();
	}

	void ObjectStore::Clear()
	{
		objects.Clear();
	}

} // namespace CE

