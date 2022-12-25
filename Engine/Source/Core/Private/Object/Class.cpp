
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

    FieldType* StructType::FindFieldWithName(Name name)
    {
        if (!fieldsCached)
            CacheAllFields();
        
        if (cachedFieldsMap.KeyExists(name))
            return cachedFieldsMap[name];
        return nullptr;
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
            
            cachedFieldsMap.Add({ cachedFields[i].GetName(), &cachedFields[i] });
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


    bool ClassType::IsObject()
    {
        if (this->GetTypeId() == TYPEID(CE::Object))
            return true;

        if (!superTypesCached)
        {
            CacheSuperTypes();
        }

        for (int i = 0; i < superTypes.GetSize(); i++)
        {
            if (superTypes[i]->IsObject())
                return true;
        }

        return false;
    }

    void ClassType::CacheSuperTypes()
    {
        if (superTypesCached)
            return;

        superTypesCached = true;

        for (int i = 0; i < superTypeIds.GetSize(); i++)
        {
            auto type = GetTypeInfo(superTypeIds[i]);
            if (type == nullptr || !type->IsClass())
                continue;

            superTypes.Add((ClassType*)type);
        }
    }

} // namespace CE
