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
        
		FORCE_INLINE Object* FindObject(UUID uuid) const
        {
			if (!objects.KeyExists(uuid))
				return nullptr;
            return objects.Get(uuid);
        }
        
		FORCE_INLINE bool ObjectExists(UUID uuid) const
        {
            return objects.KeyExists(uuid);
        }
        
        void AddObject(Object* object);
        
        void RemoveObject(Object* object);
        
        void RemoveObject(UUID uuid);

        void RemoveAll();
        
        auto begin() { return objects.begin(); }
        auto end() { return objects.end(); }

		const auto begin() const { return objects.begin(); }
		const auto end() const { return objects.end(); }

    private:
        HashMap<UUID, Object*> objects{};
    };
    
} // namespace CE

CE_RTTI_POD(CORE_API, CE, ObjectMap)
