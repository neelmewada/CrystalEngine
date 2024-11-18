
#include "Core.h"

namespace CE
{

    Bundle::Bundle()
    {

    }

    void Bundle::FetchAllSchemaTypes(Array<ClassType*>& outClasses, Array<StructType*>& outStructs)
    {
        HashSet<ClassType*> classes;
        HashSet<StructType*> structs;

        std::function<void(StructType*)> fetchInternalStructs = [&](StructType* structType)
            {
                if (structType == nullptr || structs.Exists(structType))
                    return;

                structs.Add(structType);

                for (int i = 0; i < structType->GetFieldCount(); ++i)
                {
                    FieldType* field = structType->GetFieldAt(i);

                    if (field->IsStructField())
                    {
                        Struct* childStruct = StructType::FindStruct(field->GetDeclarationTypeId());

                        fetchInternalStructs(childStruct);
                    }
                }
            };

        std::function<void(Object*)> fetchStructs = [&](Object* object)
            {
                if (object == nullptr)
                    return;

                Class* clazz = object->GetClass();

                for (int i = 0; i < clazz->GetFieldCount(); ++i)
                {
                    FieldType* field = clazz->GetFieldAt(i);

                    if (field->IsStructField())
                    {
                        Struct* structType = StructType::FindStruct(field->GetDeclarationTypeId());

                        fetchInternalStructs(structType);
                    }
                }
            };

        std::function<void(Object*)> visitor = [&](Object* object)
            {
                if (object == nullptr)
                    return;

				if (!classes.Exists(object->GetClass()))
		        {
			        classes.Add(object->GetClass());

                	fetchStructs(object);
		        }

                for (int i = 0; i < object->GetSubObjectCount(); ++i)
                {
                    visitor(object->GetSubObject(i));
                }
            };

        visitor(this);

        for (ClassType* clazz : classes)
        {
            outClasses.Add(clazz);
        }

        for (StructType* structType : structs)
        {
            outStructs.Add(structType);
        }
    }
    
} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, Bundle)