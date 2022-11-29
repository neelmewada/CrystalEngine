
#include "Object/RTTI.h"

namespace CE
{

    namespace Internal
    {
        CORE_API TypeId TypeIdGenerator::Counter = 0;
    }

    HashMap<Name, const TypeInfo*> TypeInfo::RegisteredTypes{};
    HashMap<TypeId, const TypeInfo*> TypeInfo::RegisteredTypeIds{};

    void TypeInfo::RegisterType(const TypeInfo* type)
    {
        if (type == nullptr)
            return;
        
        if (!RegisteredTypes.KeyExists(type->Name))
        {
            RegisteredTypes.Add({ type->Name, type });
        }
        if (!RegisteredTypeIds.KeyExists(type->GetTypeId()))
        {
            RegisteredTypeIds.Add({ type->GetTypeId(), type });
        }
    }

    CORE_API const TypeInfo* GetTypeInfo(Name name)
    {
        if (TypeInfo::RegisteredTypes.KeyExists(name))
        {
            return TypeInfo::RegisteredTypes[name];
        }
        return nullptr;
    }

    CORE_API const TypeInfo* GetTypeInfo(TypeId typeId)
    {
        if (TypeInfo::RegisteredTypeIds.KeyExists(typeId))
        {
            return TypeInfo::RegisteredTypeIds[typeId];
        }
        return nullptr;
    }

}

// POD Types



