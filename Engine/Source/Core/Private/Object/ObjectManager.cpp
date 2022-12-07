
#include "Object/ObjectManager.h"

namespace CE
{
    static HashMap<CE::Name, CE::Object*> ObjectStore{};

    void ObjectManager::RegisterObject(Object* object)
    {
        
    }

    void ObjectManager::DeregisterObject(Object* object)
    {
        
    }

    bool ObjectManager::IsObjectRegistered(Name name)
    {
        return true;
    }

    bool ObjectManager::IsObjectRegistered(Object* object)
    {
        return true;
    }

} // namespace CE
