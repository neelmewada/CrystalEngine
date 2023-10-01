
#include "CoreMinimal.h"

namespace CE
{
    Object* ObjectMap::FindObject(UUID uuid) const
    {
		for (auto object : objects)
		{
			if (object != nullptr && object->GetUuid() == uuid)
			{
				return object;
			}
		}
		return nullptr;
    }

    Object* ObjectMap::GetObjectAt(u32 index) const
    {
		if (index >= objects.GetSize())
			return nullptr;
		return objects[index];
    }

	bool ObjectMap::ObjectExists(UUID uuid) const
	{
		return objects.Exists([=](Object* obj) { return obj != nullptr && obj->GetUuid() == uuid; });
	}

    void ObjectMap::AddObject(Object* object)
	{
        if (object == nullptr || objects.Exists(object))
            return;
        objects.Add(object);
	}

	void ObjectMap::RemoveObject(Object* object)
	{
		if (object == nullptr)
			return;
		
		objects.Remove(object);
	}

	void ObjectMap::RemoveObject(UUID uuid)
	{
		if (!ObjectExists(uuid))
			return;

		auto objectRef = objects[uuid];
		objects.Remove(objectRef);
	}

	void ObjectMap::RemoveAll()
	{
		objects.Clear();
	}

} // namespace CE

CE_RTTI_POD_IMPL(CE, ObjectMap)

