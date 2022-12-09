
#include "Object/Class.h"


namespace CE
{

    
    
    bool StructType::IsAssignableTo(TypeId typeId) const
    {
        if (typeId == this->GetTypeId())
            return true;

        for (auto superTypeId : SuperTypeIds)
        {
            if (superTypeId == typeId)
            {
                return true;
            }

            auto superType = GetTypeInfo(superTypeId);

            if (superType != nullptr && superType->IsStruct())
            {
                auto structType = (StructType*)superType;
                if (structType->IsAssignableTo(typeId))
                    return true;
            }
            else if (superType != nullptr && superType->IsClass())
            {
                auto classType = (ClassType*)superType;
                if (classType->IsAssignableTo(typeId))
                    return true;
            }
        }

        return false;
    }

    FieldType* StructType::GetFirstField()
    {
        if (!bFieldsCached)
            CacheAllFields();
        if (CachedFields.GetSize() == 0)
            return nullptr;

        return &CachedFields[0];
    }

    void StructType::CacheAllFields()
    {
        if (bFieldsCached)
            return;

        bFieldsCached = true;

        for (int i = 0; i < SuperTypeIds.GetSize(); i++)
        {
            auto typeId = SuperTypeIds[i];

            const TypeInfo* type = GetTypeInfo(typeId);
            if (type->IsStruct())
            {
                auto structType = (StructType*)type;
                structType->CacheAllFields();
                CachedFields.AddRange(structType->CachedFields);
            }
            else if (type->IsClass())
            {
                auto classType = (ClassType*)type;
                classType->CacheAllFields();
                CachedFields.AddRange(classType->CachedFields);
            }
        }

        CachedFields.AddRange(LocalFields);

        for (int i = 0; i < CachedFields.GetSize(); i++)
        {
            CachedFields[i].Owner = this;

            if (i == CachedFields.GetSize() - 1)
            {
                CachedFields[i].Next = nullptr;
            }
            else
            {
                CachedFields[i].Next = &CachedFields[i + 1];
            }
        }
    }



} // namespace CE
