#include "CoreMinimal.h"

namespace CE
{

    JsonFieldSerializer::JsonFieldSerializer(TypeInfo* instanceType, void* instance, Stream* outStream)
        : rawInstance(instance), writer(PrettyJsonWriter::Create(outStream))
    {
        fields.Clear();
        
        if (instanceType->IsClass() || instanceType->IsStruct())
        {
            isMap = true;
            
            auto type = (StructType*)instanceType;
            
            for (auto field = type->GetFirstField(); field != nullptr; field = field->GetNext())
            {
                fields.Add(field);
            }
        }
    }

    JsonFieldSerializer::JsonFieldSerializer(Array<FieldType*> fields, void* instance, Stream* outStream)
        : rawInstance(instance), fields(fields), writer(PrettyJsonWriter::Create(outStream))
    {
        isArray = true;
    }
    
    bool JsonFieldSerializer::HasNext()
    {
        return fields.NonEmpty();
    }

    bool JsonFieldSerializer::WriteNext(CE::Stream* stream)
    {
        if (!IsValid() || !HasNext())
            return false;
        
        FieldType* field = GetNext();
        fields.Pop();
        
        if (isFirstWrite)
        {
            if (isMap)
            {
                writer.WriteObjectStart();
            }
            else if (isArray)
            {
                writer.WriteArrayStart();
            }
        }
        
        isFirstWrite = false;
        
        if (field->GetDeclarationType() == nullptr || !field->IsSerialized())
        {
            return false;
        }
        
        TypeId fieldTypeId = field->GetDeclarationType()->GetTypeId();
        
        if (field->IsIntegerField())
        {
            if (isMap)
                writer.WriteIdentifier(field->GetName().GetString());
            
            if (fieldTypeId == TYPEID(u8))
                writer.WriteValue(field->GetFieldValue<u8>(rawInstance));
            else if (fieldTypeId == TYPEID(u16))
                writer.WriteValue(field->GetFieldValue<u16>(rawInstance));
            else if (fieldTypeId == TYPEID(u32))
                writer.WriteValue(field->GetFieldValue<u32>(rawInstance));
            else if (fieldTypeId == TYPEID(u64))
                writer.WriteValue(field->GetFieldValue<u64>(rawInstance));
            else if (fieldTypeId == TYPEID(s8))
                writer.WriteValue(field->GetFieldValue<s8>(rawInstance));
            else if (fieldTypeId == TYPEID(s16))
                writer.WriteValue(field->GetFieldValue<s16>(rawInstance));
            else if (fieldTypeId == TYPEID(s32))
                writer.WriteValue(field->GetFieldValue<s32>(rawInstance));
            else if (fieldTypeId == TYPEID(s64))
                writer.WriteValue(field->GetFieldValue<s64>(rawInstance));
        }
        else if (field->IsDecimalField())
        {
            if (isMap)
                writer.WriteIdentifier(field->GetName().GetString());
            
            if (fieldTypeId == TYPEID(f32))
                writer.WriteValue(field->GetFieldValue<f32>(rawInstance));
            else if (fieldTypeId == TYPEID(f64))
                writer.WriteValue((f32)field->GetFieldValue<f64>(rawInstance));
        }
        else if (fieldTypeId == TYPEID(UUID))
        {
            
        }
        else if (fieldTypeId == TYPEID(String))
        {
            
        }
        
        if (fields.IsEmpty())
        {
            if (isMap)
            {
                writer.WriteObjectClose();
            }
            else if (isArray)
            {
                writer.WriteArrayClose();
            }
        }
        
        return true;
    }

    CE::FieldType* JsonFieldSerializer::GetNext() {
        if (!HasNext())
            return nullptr;

        return fields[0];
    }
    
    
} // namespace CE
