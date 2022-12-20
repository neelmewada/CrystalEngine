#pragma once

#include "Types/CoreTypeDefs.h"

#include "Containers/Array.h"

#include "Object/RTTIDefines.h"

namespace CE
{
    template<typename T>
    class ObjectStore
    {
    public:
        ObjectStore() : objectTypeId(TYPEID(T))
        {

        }
        
        CE_INLINE TypeId GetObjectTypeId() const
        {
            return objectTypeId;
        }

        CE_INLINE u32 GetObjectCount() const
        {
            return objects.GetSize();
        }

        CE_INLINE T* GetObjectAt(u32 index) const
        {
            return objects[index];
        }

    private:
        CE::TypeId objectTypeId = 0;
        CE::Array<T*> objects{};
    };
    
} // namespace CE
