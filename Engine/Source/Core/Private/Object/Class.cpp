
#include "Object/Class.h"


namespace CE
{

    
    
    FieldType* StructType::GetFirstField()
    {
        if (!bFieldsCached)
            CacheAllFields();

        return nullptr;
    }

    void StructType::CacheAllFields()
    {
        bFieldsCached = true;

        for (int i = 0; i < SuperTypeIds.GetSize(); i++)
        {
            auto typeId = SuperTypeIds[i];
        }
    }



} // namespace CE
