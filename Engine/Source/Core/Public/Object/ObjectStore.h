#pragma once

#include "Object/Object.h"

namespace CE
{

    template<typename T>
    class ObjectStore
    {
    public:
        ObjectStore() : elementTypeId(TYPEID(T))
        {

        }

        CE_INLINE u32 GetObjectCount() const
        {
            return objects.GetSize();
        }

        CE_INLINE T* GetObjectAt(u32 index) const
        {
            return objects[index];
        }

        CE_INLINE TypeId GetElementTypeId() const
        {
            return elementTypeId;
        }

    private:
        TypeId elementTypeId = 0;
        CE::Array<T*> objects{};
    };
    
} // namespace CE
