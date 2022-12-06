
#include "Object/ObjectManager.h"

namespace CE
{
    static HashMap<CE::Name, CE::Object*> ObjectStore{};

    void ObjectManager::RegisterObject(Object* object)
    {
        if (object == nullptr)
            return;

        ObjectStore.Add({ object->GetName(), object });
    }

    void ObjectManager::DeregisterObject(Object* object)
    {
        if (object == nullptr)
            return;

        ObjectStore.Remove(object->GetName());
    }

    bool ObjectManager::IsObjectRegistered(Name name)
    {
        return ObjectStore.KeyExists(name);
    }

    bool ObjectManager::IsObjectRegistered(Object* object)
    {
        if (object == nullptr)
            return false;

        return ObjectStore.KeyExists(object->GetName());
    }

} // namespace CE
