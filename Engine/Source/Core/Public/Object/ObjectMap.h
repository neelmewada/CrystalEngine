#pragma once

#include "Types/CoreTypeDefs.h"

#include "RTTI/RTTIDefines.h"

#include "Containers/Array.h"
#include "Containers/HashMap.h"

#include "Logger/Logger.h"

namespace CE
{
    class Object;

    struct CORE_API ObjectMap
    {
    public:
        ObjectMap() {}

        FORCE_INLINE u32 GetObjectCount() const
        {
            return (u32)objects.GetSize();
        }
        
		Object* FindObject(UUID uuid) const;

		Object* GetObjectAt(u32 index) const;
        
		bool ObjectExists(UUID uuid) const;
        
        void AddObject(Object* object);
        
        void RemoveObject(Object* object);
        
        void RemoveObject(UUID uuid);

        void RemoveAll();
        
        auto begin() { return objects.begin(); }
        auto end() { return objects.end(); }

		const auto begin() const { return objects.begin(); }
		const auto end() const { return objects.end(); }

    private:
        //HashMap<UUID, Object*> objects{};
		Array<Object*> objects{};
    };
    
} // namespace CE

CE_RTTI_POD(CORE_API, CE, ObjectMap)
