
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
			*stream << field->GetDeclarationType()->GetTypeName();
		}
        
        auto dataSizePos = stream->GetCurrentPosition();
		if (!skipHeader)
			*stream << (u32)0; // Set 0 size for now. Updated later
        
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
		else if (fieldTypeId == TYPEID(UUID))
		{
			*stream << field->GetFieldValue<UUID>(rawInstance);
		}
		else if (fieldTypeId == TYPEID(String))
		{
			*stream << field->GetFieldValue<String>(rawInstance);
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

			*stream << underlyingType->GetTypeName();
			*stream << arraySize;

			u64 dataSizePos = stream->GetCurrentPosition();
			*stream << (u32)0; // Total data byte size, excluding itself

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

			u64 curPos = stream->GetCurrentPosition();
			stream->Seek(dataSizePos);
			*stream << (u32)(curPos - dataSizePos - sizeof(u32)); // subtract the size of the "Data Byte Size" field itself
			stream->Seek(curPos);
		}
		else if (fieldTypeId == TYPEID(ObjectMap)) // ObjectMaps are stored just like arrays
		{
			const auto& map = field->GetFieldValue<ObjectMap>(rawInstance);

			u32 mapSize = map.GetObjectCount();

			*stream << TYPENAME(Object);
			*stream << mapSize;

			u64 dataSizePos = stream->GetCurrentPosition();
			*stream << (u32)0; // Total data byte size, excluding itself

			for (const auto& [uuid, object] : map)
			{
				if (object == nullptr)
					continue;

				WriteObjectReference(stream, object);
			}

			u64 curPos = stream->GetCurrentPosition();
			stream->Seek(dataSizePos);
			*stream << (u32)(curPos - dataSizePos - sizeof(u32)); // subtract the size of the "Data Byte Size" field itself
			stream->Seek(curPos);
		}
        else if (field->IsObjectField())
        {
            Object* object = field->GetFieldValue<Object*>(rawInstance);
            
			WriteObjectReference(stream, object);
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

	void FieldSerializer::WriteObjectReference(Stream* stream, Object* objectRef)
	{
		if (objectRef != nullptr && !objectRef->IsTransient()) // non-temporary object
		{
			*stream << objectRef->GetUuid();
			*stream << objectRef->GetClass()->GetTypeName();

			auto package = objectRef->GetPackage();

			if (package != nullptr)
				*stream << package->GetPackageName();
			else
				*stream << "\0";

			*stream << objectRef->GetPathInPackage();
		}
		else
		{
			*stream << (u64)0; // A Uuid of 0 means NULL object
		}
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

	FieldDeserializer::FieldDeserializer(FieldType* fieldChain, void* instance, Package* currentPackage)
		: rawInstance(instance), skipHeader(false), currentPackage(currentPackage)
	{
		fields.Clear();

		FieldType* field = fieldChain;

		while (field != nullptr)
		{
			fields.Add(field);

			field = field->GetNext();
		}
	}

	FieldDeserializer::FieldDeserializer(Array<FieldType*> fieldList, void* instance, Package* currentPackage)
        : fields(fieldList), rawInstance(instance), skipHeader(false), currentPackage(currentPackage)
    {
        
    }

	bool FieldDeserializer::HasNext()
	{
		return fields.NonEmpty();
	}

	bool FieldDeserializer::ReadNext(Stream* stream)
    {
		String fieldName = "";
		Name fieldTypeName = "";
		u32 fieldDataSize = 0;

		FieldType* field = nullptr;

		if (!skipHeader)
		{
			*stream >> fieldName;
			if (fieldName.IsEmpty())
			{
				fields.Clear(); // Reached end of list
				return false;
			}

			*stream >> fieldTypeName;
			*stream >> fieldDataSize;

			s32 index = fields.IndexOf([&](FieldType* field) { return field->GetName() == fieldName; });

			if (index < 0 || index >= fields.GetSize() || fieldDataSize == 0)
			{
				stream->Seek(fieldDataSize, SeekMode::Current); // skip this field entry
				return false;
			}

			field = fields[index];
			fields.RemoveAt(index);

			if (field == nullptr) // Field found in serialized data but not actually present in class/struct
			{
				stream->Seek(fieldDataSize, SeekMode::Current);
				return false;
			}
			if (!fieldTypeName.IsValid()) // Invalid serialized type name
			{
				stream->Seek(fieldDataSize, SeekMode::Current);
				return false;
			}
			if (fieldTypeName != field->GetTypeName()) // Type mismatch
			{
				stream->Seek(fieldDataSize, SeekMode::Current);
				return false;
			}
		}
		else
		{
			field = fields[0];
			fields.RemoveAt(0);
		}

		TypeId fieldTypeId = field->GetTypeId();

		if (field->IsIntegerField())
		{
			if (fieldTypeId == TYPEID(u8))
				*stream >> field->GetFieldValue<u8>(rawInstance);
			else if (fieldTypeId == TYPEID(u16))
				*stream >> field->GetFieldValue<u16>(rawInstance);
			else if (fieldTypeId == TYPEID(u32))
				*stream >> field->GetFieldValue<u32>(rawInstance);
			else if (fieldTypeId == TYPEID(u64))
				*stream >> field->GetFieldValue<u64>(rawInstance);
			else if (fieldTypeId == TYPEID(s8))
				*stream >> field->GetFieldValue<s8>(rawInstance);
			else if (fieldTypeId == TYPEID(s16))
				*stream >> field->GetFieldValue<s16>(rawInstance);
			else if (fieldTypeId == TYPEID(s32))
				*stream >> field->GetFieldValue<s32>(rawInstance);
			else if (fieldTypeId == TYPEID(s64))
				*stream >> field->GetFieldValue<s64>(rawInstance);
		}
		else if (field->IsDecimalField())
		{
			f32 value = 0;
			*stream >> value;
			if (fieldTypeId == TYPEID(f32))
				field->SetFieldValue<f32>(rawInstance, value);
			else if (fieldTypeId == TYPEID(f64))
				field->SetFieldValue<f64>(rawInstance, value);
		}
		else if (fieldTypeId == TYPEID(UUID))
		{
			*stream >> field->GetFieldValue<UUID>(rawInstance);
		}
		else if (fieldTypeId == TYPEID(String))
		{
			*stream >> field->GetFieldValue<String>(rawInstance);
		}
		else if (fieldTypeId == TYPEID(Name))
		{
			*stream >> field->GetFieldValue<Name>(rawInstance);
		}
		else if (fieldTypeId == TYPEID(IO::Path))
		{
			String pathString{};
			*stream >> pathString;
			field->SetFieldValue<IO::Path>(rawInstance, pathString);
		}
		else if (field->IsArrayField())
		{
			auto& array = field->GetFieldValue<Array<u8>>(rawInstance);
			auto underlyingType = field->GetUnderlyingType();

			Name serializedUnderlyingTypeName{};
			u32 numElements = 0;
			u32 dataSize = 0;

			*stream >> serializedUnderlyingTypeName;
			*stream >> numElements;
			*stream >> dataSize;

			if (underlyingType->GetName() != serializedUnderlyingTypeName) // Underlying type mismatch
			{
				stream->Seek(dataSize, SeekMode::Current);
				return false;
			}
			
			array.Clear();

			if (numElements > 0)
			{
				field->ResizeArray(rawInstance, numElements);

				Array<FieldType> fieldList = field->GetArrayFieldList(rawInstance);
				Array<FieldType*> fieldListPtr = fieldList.Transform<FieldType*>([](FieldType& item)-> FieldType*
					{
						return &item;
					});

				void* arrayInstance = &array[0];

				FieldDeserializer fieldDeserializer{ fieldListPtr, arrayInstance, currentPackage };
				fieldDeserializer.SkipHeader(true);
				while (fieldDeserializer.HasNext())
				{
					fieldDeserializer.ReadNext(stream);
				}
			}
		}
		else if (fieldTypeId == TYPEID(ObjectMap))
		{
			ObjectMap& map = field->GetFieldValue<ObjectMap>(rawInstance);

			Name serializedUnderlyingTypeName{};
			u32 numElements = 0;
			u32 dataSize = 0;

			*stream >> serializedUnderlyingTypeName;
			*stream >> numElements;
			*stream >> dataSize;

			map.RemoveAll();

			if (numElements > 0)
			{
				for (int i = 0; i < numElements; i++)
				{
                    Object* object = ReadObjectReference(stream);
                    if (object == nullptr)
                        continue;
                    map.AddObject(object);
				}
			}
		}
		else if (field->IsObjectField())
		{
            field->SetFieldValue(rawInstance, ReadObjectReference(stream));
		}

        return true;
    }

	Object* FieldDeserializer::ReadObjectReference(Stream* stream)
	{
		u64 uuid = 0;
		*stream >> uuid;
		if (uuid == 0) // Uuid of 0 means nullptr, skip this entry
		{
			return nullptr;
		}

		Name objectTypeName{};
		*stream >> objectTypeName;
		Name packageName{};
		*stream >> packageName;
		Name pathInPackage{};
		*stream >> pathInPackage;

		if (currentPackage != nullptr && packageName == currentPackage->GetPackageName())
		{
			return currentPackage->ResolveObjectReference(uuid);
		}
		else
		{
			// TODO: Loading references from external packages
			ResolveObjectReference(uuid, packageName, pathInPackage);
			return nullptr;
		}
	}

	Object* FieldDeserializer::ResolveObjectReference(UUID objectUuid, Name packageName, Name pathInPackage)
	{
		Package* package = Package::LoadPackage(nullptr, packageName);
        if (package == nullptr)
            return nullptr;
        
        return package->LoadObject(objectUuid);
	}

}
