
#include "Object/RTTI.h"

namespace CE
{

    HashMap<String, const TypeInfo*> TypeInfo::RegisteredTypes{};

    void TypeInfo::RegisterType(const TypeInfo* type)
    {
        RegisteredTypes.Add({type->Name, type});
    }

    const TypeInfo* GetStaticType(String namePath)
    {
        if (TypeInfo::RegisteredTypes.KeyExists(namePath))
        {
            return TypeInfo::RegisteredTypes[namePath];
        }
        return nullptr;
    }

}

