#pragma once

#include "Types/CoreTypeDefs.h"

#include "Containers/Array.h"
#include "Containers/HashMap.h"

#include "Object/RTTIDefines.h"

namespace CE
{
    class Object;

    class ObjectStore
    {
    public:
        ObjectStore()
        {

        }

        CE_INLINE u32 GetObjectCount() const
        {
            return (u32)objects.GetSize();
        }

        CE_INLINE Object* GetObjectAt(u32 index) const
        {
            return objects[index];
        }
        
        CE_INLINE Object* RemoveObjectAt(u32 index)
        {
            auto obj = objects[index];
            objects.RemoveAt(index);
            if (obj != nullptr)
            {
                addedObjects.Remove(obj->GetUuid());
            }
            return obj;
        }
        
        CE_INLINE void AddObject(Object* object)
        {
            if (object == nullptr)
                return;
            if (addedObjects.KeyExists(object->GetUuid()))
                return;
            
            objects.Add(object);
            addedObjects.Add({object->GetUuid(), object});
        }
        
        CE_INLINE void RemoveObject(Object* object)
        {
            if (object == nullptr)
                return;
            
            objects.Remove(object);
            addedObjects.Remove(object->GetUuid());
        }
        
        CE_INLINE void Clear()
        {
            objects.Clear();
            addedObjects.Clear();
        }

    private:
        Array<Object*> objects{};
        HashMap<UUID, Object*> addedObjects{};
    };
    
} // namespace CE
