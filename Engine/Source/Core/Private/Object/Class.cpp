
#include "Object/Class.h"


namespace CE
{

    
    
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
