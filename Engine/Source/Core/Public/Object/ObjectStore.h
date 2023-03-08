#pragma once

#include "Types/CoreTypeDefs.h"

#include "Object/RTTIDefines.h"

#include "Containers/Array.h"
#include "Containers/HashMap.h"

#include "Logger/Logger.h"

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
        
        CE_INLINE Object* GetObjectWithUuid(UUID uuid)
        {
            return objects[uuid];
        }
        
        CE_INLINE bool ObjectExistsWithUuid(UUID uuid) const
        {
            return objects.KeyExists(uuid);
        }
        
        void AddObject(Object* object)
        {
            if (object == nullptr)
                return;
            if (objects.KeyExists(object->GetUuid()))
                return;
            
            objects.Add({object->GetUuid(), object});
            objectsArray.Add(object);
        }
        
        void RemoveObject(Object* object)
        {
            if (object == nullptr)
                return;
            
            objects.Remove(object->GetUuid());
            objectsArray.Remove(object);
        }
        
        void RemoveObjectWithUuid(UUID uuid)
        {
            if (!objects.KeyExists(uuid))
                return;

            auto objectRef = objects[uuid];
            objects.Remove(uuid);
            objectsArray.Remove(objectRef);
        }

        void DestroyAll()
        {
            for (auto object : objectsArray)
            {
                delete object;
            }
            Clear();
        }
        
        void Clear()
        {
            objects.Clear();
            objectsArray.Clear();
        }
        
        auto begin() { return objects.begin(); }
        auto end() { return objects.end(); }

    private:
        HashMap<UUID, Object*> objects{};
        CE::Array<Object*> objectsArray{};
    };
    
} // namespace CE
