
#include "CoreMinimal.h"

namespace CE
{

	Ref<Object> ObjectMap::FindObject(Uuid uuid) const
    {
		for (const auto& object : objects)
		{
			if (object != nullptr && object->GetUuid() == uuid)
			{
				return object;
			}
		}
		return nullptr;
    }

	Ref<Object> ObjectMap::FindObject(const Name& name, ClassType* classType) const
	{
		for (const auto& object : objects)
		{
			if (object != nullptr && object->GetName() == name && (classType == nullptr || classType == object->GetClass()))
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

	bool ObjectMap::ObjectExists(Uuid uuid) const
	{
		return objects.Exists([=](Object* obj) { return obj != nullptr && obj->GetUuid() == uuid; });
	}

	bool ObjectMap::ObjectExists(const Name& objectName) const
	{
		return objects.Exists([=](Object* obj) { return obj != nullptr && obj->GetName() == objectName; });
	}

    void ObjectMap::AddObject(Object* object)
	{
        if (object == nullptr || objects.Exists([&](const Ref<Object>& obj) { return obj.Get() == object; }))
            return;
        objects.Add(object);
	}

	void ObjectMap::RemoveObject(Object* object)
	{
		if (object == nullptr)
			return;
		
		objects.Remove(object);
	}

	void ObjectMap::RemoveObject(Uuid uuid)
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

