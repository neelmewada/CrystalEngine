
#include "CoreMinimal.h"

namespace CE
{

	FieldSerializer::FieldSerializer(FieldType* fieldChain, void* instance)
        : rawInstance(instance)
	{
		fields.Clear();

		FieldType* field = fieldChain;

		while (field != nullptr)
		{
			fields.Add(field);
            
			field = field->GetNext();
		}
	}

	FieldSerializer::FieldSerializer(Array<FieldType*> fieldList, void* instance)
        : rawInstance(instance), fields(fieldList)
	{

	}

	bool FieldSerializer::HasNext()
	{
		return fields.NonEmpty();
	}

	bool FieldSerializer::WriteNext(Stream* stream)
	{
		if (stream == nullptr || !stream->CanWrite() || rawInstance == nullptr || !HasNext())
			return false;

        FieldType* field = fields[0];
		TypeId fieldTypeId = field->GetDeclarationTypeId();
		TypeInfo* fieldDeclarationType = field->GetDeclarationType();
        
        if (fieldDeclarationType == nullptr || fieldTypeId == 0 || !field->IsSerialized())
        {
            fields.RemoveAt(0);
            return false;
        }
        
		if (!skipHeader)
		{
			*stream << field->GetName();
			*stream << field->GetDeclarationType()->GetName();
		}
        
        auto dataSizePos = stream->GetCurrentPosition();
		if (!skipHeader)
			*stream << (u32)0; // Set 0 size for now
        
        auto dataStartPos = stream->GetCurrentPosition();
		
		if (field->IsIntegerField())
		{
			if (fieldTypeId == TYPEID(u8))
				*stream << field->GetFieldValue<u8>(rawInstance);
			else if (fieldTypeId == TYPEID(u16))
				*stream << field->GetFieldValue<u16>(rawInstance);
			else if (fieldTypeId == TYPEID(u32))
				*stream << field->GetFieldValue<u32>(rawInstance);
			else if (fieldTypeId == TYPEID(u64))
				*stream << field->GetFieldValue<u64>(rawInstance);
			else if (fieldTypeId == TYPEID(s8))
				*stream << field->GetFieldValue<s8>(rawInstance);
			else if (fieldTypeId == TYPEID(s16))
				*stream << field->GetFieldValue<s16>(rawInstance);
			else if (fieldTypeId == TYPEID(s32))
				*stream << field->GetFieldValue<s32>(rawInstance);
			else if (fieldTypeId == TYPEID(s64))
				*stream << field->GetFieldValue<s64>(rawInstance);
		}
		else if (field->IsDecimalField())
		{
			if (fieldTypeId == TYPEID(f32))
				*stream << field->GetFieldValue<f32>(rawInstance);
			else if (fieldTypeId == TYPEID(f64))
				*stream << (f32)field->GetFieldValue<f64>(rawInstance);
		}
		else if (fieldTypeId == TYPEID(String))
		{
			*stream << field->GetFieldValue<String>(rawInstance);
		}
		else if (fieldTypeId == TYPEID(UUID))
		{
			*stream << field->GetFieldValue<UUID>(rawInstance);
		}
		else if (fieldTypeId == TYPEID(Name))
		{
			*stream << field->GetFieldValue<Name>(rawInstance);
		}
		else if (fieldTypeId == TYPEID(IO::Path))
		{
			*stream << field->GetFieldValue<IO::Path>(rawInstance).GetString();
		}
		else if (field->IsArrayField())
		{
			auto& array = field->GetFieldValue<Array<u8>>(rawInstance);
			
			auto underlyingType = field->GetUnderlyingType();
			u32 arraySize = field->GetArraySize(rawInstance);

			*stream << underlyingType->GetName();
			*stream << arraySize;

			Array<FieldType> fieldList = field->GetArrayFieldList(rawInstance);
			Array<FieldType*> fieldListPtr = fieldList.Transform<FieldType*>([](FieldType& item)-> FieldType*
			{
				return &item;
			});

			if (arraySize > 0)
			{
				void* arrayInstance = &array[0];

				FieldSerializer fieldSerializer{ fieldListPtr, arrayInstance };
				fieldSerializer.SkipHeader(true);
				while (fieldSerializer.HasNext())
				{
					fieldSerializer.WriteNext(stream);
				}
			}
		}
        else if (field->IsObjectField())
        {
            Object* object = field->GetFieldValue<Object*>(rawInstance);
            
            if (object != nullptr && !object->IsTransient()) // non-temporary object
            {
                *stream << object->GetUuid();
                *stream << object->GetClass()->GetName();
                
                auto package = object->GetPackage();
                
                if (package != nullptr)
                    *stream << package->GetName();
                else
                    *stream << "\0";
                
                *stream << object->GetPathInPackage();
            }
            else
            {
                *stream << (u64)0; // A Uuid of 0 means NULL object
            }
        }
		else if (fieldDeclarationType->IsStruct())
		{
			StructType* structType = (StructType*)fieldDeclarationType;
			auto structInstance = field->GetFieldInstance(rawInstance);

			auto firstField = structType->GetFirstField();

			FieldSerializer fieldSerializer{ firstField, structInstance };
			while (fieldSerializer.HasNext())
			{
				fieldSerializer.WriteNext(stream);
			}
		}
        
        u64 curPos = stream->GetCurrentPosition();
        auto dataSize = (u32)(stream->GetCurrentPosition() - dataStartPos);
        
		if (!skipHeader)
		{
			stream->Seek(dataSizePos);
			*stream << dataSize;
			stream->Seek(curPos);
		}

		fields.RemoveAt(0);
		return true;
	}

	FieldType* FieldSerializer::GetNext()
	{
		if (!HasNext())
			return nullptr;

        return fields[0];
	}

	void FieldSerializer::SkipHeader(bool skip)
	{
		this->skipHeader = skip;
	}

}
