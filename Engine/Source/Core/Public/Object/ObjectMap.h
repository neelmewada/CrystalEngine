#pragma once

#include "Types/CoreTypeDefs.h"

#include "RTTI/RTTIDefines.h"

#include "Containers/Array.h"
#include "Containers/HashMap.h"

#include "Logger/Logger.h"

namespace CE
{
    class Object;

    class CORE_API ObjectMap
    {
    public:
        ObjectMap()
        {

        }

        CE_INLINE u32 GetObjectCount() const
        {
            return (u32)objects.GetSize();
        }
        
        CE_INLINE Object* FindObject(UUID uuid)
        {
            return objects[uuid];
        }
        
        CE_INLINE bool ObjectExists(UUID uuid) const
        {
            return objects.KeyExists(uuid);
        }
        
        void AddObject(Object* object);
        
        void RemoveObject(Object* object);
        
        void RemoveObject(UUID uuid);

        void RemoveAll();
        
        auto begin() { return objects.begin(); }
        auto end() { return objects.end(); }

    private:
        HashMap<UUID, Object*> objects{};
    };
    
} // namespace CE
