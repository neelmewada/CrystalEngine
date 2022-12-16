
#include "Object/Class.h"


namespace CE
{

    
    
    bool StructType::IsAssignableTo(TypeId typeId) const
    {
        if (typeId == this->GetTypeId())
            return true;

        for (auto superTypeId : superTypeIds)
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
        if (!fieldsCached)
            CacheAllFields();
        if (cachedFields.GetSize() == 0)
            return nullptr;

        return &cachedFields[0];
    }

    void StructType::CacheAllFields()
    {
        if (fieldsCached)
            return;

        fieldsCached = true;

        for (int i = 0; i < superTypeIds.GetSize(); i++)
        {
            auto typeId = superTypeIds[i];
            if (typeId == this->GetTypeId())
                continue;

            const TypeInfo* type = GetTypeInfo(typeId);
            if (type->IsStruct())
            {
                auto structType = (StructType*)type;
                structType->CacheAllFields();
                cachedFields.AddRange(structType->cachedFields);
            }
            else if (type->IsClass())
            {
                auto classType = (ClassType*)type;
                classType->CacheAllFields();
                cachedFields.AddRange(classType->cachedFields);
            }
        }

        cachedFields.AddRange(localFields);

        for (int i = 0; i < cachedFields.GetSize(); i++)
        {
            cachedFields[i].owner = this;

            if (i == cachedFields.GetSize() - 1)
            {
                cachedFields[i].next = nullptr;
            }
            else
            {
                cachedFields[i].next = &cachedFields[i + 1];
            }
        }
    }

    void StructType::CacheAllFunctions()
    {
        if (functionsCached)
            return;

        functionsCached = true;

        for (int i = 0; i < superTypeIds.GetSize(); i++)
        {
            auto typeId = superTypeIds[i];
            if (typeId == this->GetTypeId())
                continue;

            const TypeInfo* type = GetTypeInfo(typeId);
            if (type->IsStruct())
            {
                auto structType = (StructType*)type;
                structType->CacheAllFunctions();
                
            }
            else if (type->IsClass())
            {
                auto classType = (ClassType*)type;
                classType->CacheAllFunctions();

            }
        }
    }



} // namespace CE
