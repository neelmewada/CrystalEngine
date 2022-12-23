
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

    const TypeInfo* FieldType::GetDeclarationType() const
    {
        return GetTypeInfo(fieldTypeId);
    }

}
