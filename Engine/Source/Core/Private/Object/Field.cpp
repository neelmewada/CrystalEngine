
#include "Object/Field.h"

namespace CE
{
    
    bool FieldType::IsAssignableTo(TypeId typeId) const
    {
        const auto declType = GetDeclarationType();
        
        if (declType == nullptr)
            return false;
        
        if (declType->IsAssignableTo(typeId))
            return true;
        
        return false;
    }

    bool FieldType::IsSerialized() const
    {
        return !attributes.Exists([](const CE::Attribute& attr) -> bool
            {
                return attr.GetKey() == "NonSerialized";
            }
        );
    }

    bool FieldType::IsHidden() const
    {
        return attributes.Exists([](const CE::Attribute& attr) -> bool
            {
                return attr.GetKey() == "Hidden";
            }
        );
    }

    const TypeInfo* FieldType::GetDeclarationType() const
    {
        return GetTypeInfo(fieldTypeId);
    }

}
