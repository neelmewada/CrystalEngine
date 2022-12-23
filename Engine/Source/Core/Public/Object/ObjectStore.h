#pragma once

#include "Types/CoreTypeDefs.h"

#include "Containers/Array.h"
#include "Containers/HashSet.h"

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
            addedObjects.Remove(obj);
            return obj;
        }
        
        CE_INLINE void AddObject(Object* object)
        {
            if (addedObjects.Exists(object))
                return;
            
            objects.Add(object);
            addedObjects.Add(object);
        }
        
        CE_INLINE void RemoveObject(Object* object)
        {
            objects.Remove(object);
            addedObjects.Remove(object);
        }
        
        CE_INLINE void Clear()
        {
            objects.Clear();
            addedObjects.Clear();
        }

    private:
        Array<Object*> objects{};
        HashSet<Object*> addedObjects{};
    };
    
} // namespace CE
