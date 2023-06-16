#include "CoreMinimal.h"

namespace CE
{

    JsonFieldSerializer::JsonFieldSerializer(StructType* instanceType, void* instance)
        : rawInstance(instance), writer(PrettyJsonWriter::Create(nullptr))
    {
        fields.Clear();

        isMap = true;
            
        auto type = (StructType*)instanceType;
            
        for (auto field = type->GetFirstField(); field != nullptr; field = field->GetNext())
        {
            fields.Add(field);
        }
    }

    JsonFieldSerializer::JsonFieldSerializer(Array<FieldType*> fields, void* instance)
        : rawInstance(instance), fields(fields), writer(PrettyJsonWriter::Create(nullptr))
    {
        isArray = true;
    }
    
    bool JsonFieldSerializer::HasNext()
    {
        return fields.NonEmpty();
    }

    bool JsonFieldSerializer::WriteNext(Stream* stream)
    {
        if (!IsValid() || !HasNext())
            return false;
        
        FieldType* field = GetNext();
		fields.RemoveAt(0);
        
        if (isFirstWrite)
        {
			if (writer.GetStream() == nullptr)
				writer = PrettyJsonWriter::Create(stream);

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
        
		TypeInfo* fieldDeclType = field->GetDeclarationType();
        TypeId fieldTypeId = fieldDeclType->GetTypeId();
        
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
			if (isMap)
				writer.WriteIdentifier(field->GetName().GetString());

			writer.WriteValue((u64)field->GetFieldValue<UUID>(rawInstance));
        }
        else if (fieldTypeId == TYPEID(String))
        {
			if (isMap)
				writer.WriteIdentifier(field->GetName().GetString());

			const String& string = field->GetFieldValue<String>(rawInstance);
			writer.WriteValue(string);
        }
		else if (fieldTypeId == TYPEID(Name))
		{
			if (isMap)
				writer.WriteIdentifier(field->GetName().GetString());

			writer.WriteValue(field->GetFieldValue<Name>(rawInstance).GetString());
		}
		else if (fieldDeclType->IsStruct())
		{
			StructType* structType = (StructType*)fieldDeclType;

			if (isMap)
				writer.WriteIdentifier(field->GetName().GetString());

			JsonFieldSerializer serializer{ structType, field->GetFieldInstance(rawInstance) };
			serializer.writer = this->writer;

			if (serializer.HasNext())
			{
				while (serializer.HasNext())
				{
					serializer.WriteNext(stream);
				}
			}
			else
			{
				writer.WriteObjectStart();
				writer.WriteObjectClose();
			}

			this->writer = serializer.writer;
		}
		else if (field->IsArrayField())
		{
			Array<FieldType> fieldsList = field->GetArrayFieldList(rawInstance);
			Array<FieldType*> fieldListPtr = fieldsList.Transform<FieldType*>([](FieldType& in) { return &in; });

			Array<u8> array = field->GetFieldValue<Array<u8>>(rawInstance);

			if (array.IsEmpty())
			{
				if (isMap)
					writer.WriteIdentifier(field->GetName().GetString());

				writer.WriteArrayStart();
				writer.WriteArrayClose();
			}
			else
			{
				if (isMap)
					writer.WriteIdentifier(field->GetName().GetString());

				JsonFieldSerializer serializer{ fieldListPtr, &array[0] };
				serializer.writer = this->writer;

				while (serializer.HasNext())
				{
					serializer.WriteNext(stream);
				}

				this->writer = serializer.writer;
			}
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

    FieldType* JsonFieldSerializer::GetNext()
	{
        if (!HasNext())
            return nullptr;

        return fields[0];
    }
    
    
	JsonFieldDeserializer::JsonFieldDeserializer(StructType* instanceType, void* instance)
		: rawInstance(instance)
	{
		fields.Clear();

		isMap = true;

		auto type = (StructType*)instanceType;

		for (auto field = type->GetFirstField(); field != nullptr; field = field->GetNext())
		{
			fields.Add(field);
		}
	}

	JsonFieldDeserializer::JsonFieldDeserializer(Array<FieldType*> fields, void* instance)
		: rawInstance(instance), fields(fields)
	{
		isArray = true;
	}

	JsonFieldDeserializer::~JsonFieldDeserializer()
	{
		delete rootJson;
		rootJson = nullptr;
	}

	bool JsonFieldDeserializer::HasNext()
	{
		return fields.NonEmpty();
	}

	bool JsonFieldDeserializer::ReadNext(Stream* stream)
	{
		if (!IsValid() || !HasNext())
			return false;

		if (isFirstRead && rootJson == nullptr)
		{
			rootJson = JsonSerializer::Deserialize(stream);
		}

		isFirstRead = false;

		if (rootJson == nullptr)
			return false;

		if (rootJson->IsObjectValue() && isMap)
		{
			JsonObject& objectValue = rootJson->GetObjectValue();
			for (const auto& [key, value] : objectValue)
			{
				s32 idx = fields.IndexOf([&](FieldType* fieldType) { return fieldType->GetName() == key; });
				if (idx < 0)
					continue;

				FieldType* field = fields[idx];
				fields.RemoveAt(idx);

				auto result = ReadField(field, value);

				delete value;
				objectValue.Remove(key);
				return result;
			}
		}
		else if (rootJson->IsArrayValue() && isArray)
		{
			JsonArray& arrayValue = rootJson->GetArrayValue();

			if (arrayValue.IsEmpty() || fields.IsEmpty())
			{
				return false;
			}

			FieldType* field = fields[0];
			fields.RemoveAt(0);
			
			auto result = ReadField(field, arrayValue[0]);

			delete arrayValue[0];
			arrayValue.RemoveAt(0);
			return result;
		}
		else
		{
			return false;
		}

		return true;
	}

	bool JsonFieldDeserializer::ReadField(FieldType* field, JsonValue* jsonValue)
	{
		if (field == nullptr || jsonValue == nullptr || field->GetDeclarationType() == nullptr)
			return false;

		auto fieldDeclType = field->GetDeclarationType();
		auto fieldTypeId = field->GetDeclarationTypeId();

		if (jsonValue->IsNumberValue() &&
			(field->IsIntegerField() || field->IsDecimalField() || fieldTypeId == TYPEID(UUID)))
		{
			if (fieldTypeId == TYPEID(s8))
				field->SetFieldValue<s8>(rawInstance, (s8)jsonValue->GetNumberValue());
			else if (fieldTypeId == TYPEID(s16))
				field->SetFieldValue<s16>(rawInstance, (s16)jsonValue->GetNumberValue());
			else if (fieldTypeId == TYPEID(s32))
				field->SetFieldValue<s32>(rawInstance, (s32)jsonValue->GetNumberValue());
			else if (fieldTypeId == TYPEID(s64))
				field->SetFieldValue<s64>(rawInstance, (s64)jsonValue->GetNumberValue());
			else if (fieldTypeId == TYPEID(u8))
				field->SetFieldValue<u8>(rawInstance, (u8)jsonValue->GetNumberValue());
			else if (fieldTypeId == TYPEID(u16))
				field->SetFieldValue<u16>(rawInstance, (u16)jsonValue->GetNumberValue());
			else if (fieldTypeId == TYPEID(u32))
				field->SetFieldValue<u32>(rawInstance, (u32)jsonValue->GetNumberValue());
			else if (fieldTypeId == TYPEID(u64))
				field->SetFieldValue<u64>(rawInstance, (u64)jsonValue->GetNumberValue());
			else if (fieldTypeId == TYPEID(f32))
				field->SetFieldValue<f32>(rawInstance, (f32)jsonValue->GetNumberValue());
			else if (fieldTypeId == TYPEID(f64))
				field->SetFieldValue<f64>(rawInstance, (f64)jsonValue->GetNumberValue());
			else if (fieldTypeId == TYPEID(UUID))
				field->SetFieldValue<UUID>(rawInstance, UUID((u64)jsonValue->GetNumberValue()));
			else
				return false;

			return true;
		}
		else if (jsonValue->IsBoolValue())
		{
			if (fieldTypeId == TYPEID(bool))
				field->SetFieldValue<bool>(rawInstance, (bool)jsonValue->GetBoolValue());
			else if (fieldTypeId == TYPEID(b8))
				field->SetFieldValue<b8>(rawInstance, (b8)jsonValue->GetBoolValue());
			else
				return false;

			return true;
		}
		else if (jsonValue->IsStringValue())
		{
			if (fieldTypeId == TYPEID(String))
				field->SetFieldValue<String>(rawInstance, jsonValue->GetStringValue());
			else if (fieldTypeId == TYPEID(Name))
				field->SetFieldValue<Name>(rawInstance, jsonValue->GetStringValue());
			else if (fieldTypeId == TYPEID(IO::Path))
				field->SetFieldValue<IO::Path>(rawInstance, IO::Path(jsonValue->GetStringValue()));
			else
				return false;

			return true;
		}
		else if (jsonValue->IsObjectValue() && fieldDeclType->IsStruct())
		{
			void* structInstance = field->GetFieldInstance(rawInstance);

			JsonFieldDeserializer deserializer{ (StructType*)fieldDeclType, structInstance };
			deserializer.isFirstRead = false;
			deserializer.rootJson = jsonValue;
			deserializer.isMap = true;
			deserializer.isArray = false;

			while (deserializer.HasNext())
			{
				deserializer.ReadNext(nullptr);
			}

			deserializer.rootJson = nullptr; // Unset json to prevent deletion

			return true;
		}
		else if (jsonValue->IsArrayValue() && fieldDeclType->IsArrayType())
		{
			if (jsonValue->GetArrayValue().IsEmpty())
			{
				field->ResizeArray(rawInstance, 0);
				return true;
			}

			field->ResizeArray(rawInstance, jsonValue->GetArrayValue().GetSize());
			Array<FieldType> fieldList = field->GetArrayFieldList(rawInstance);
			Array<FieldType*> fieldListPtr = fieldList.Transform<FieldType*>([](FieldType& f) { return &f; });

			auto arrayInstance = &field->GetFieldValue<Array<u8>>(rawInstance)[0];

			JsonFieldDeserializer deserializer{ fieldListPtr, arrayInstance };
			deserializer.isFirstRead = false;
			deserializer.rootJson = jsonValue;
			deserializer.isMap = false;
			deserializer.isArray = true;

			while (deserializer.HasNext())
			{
				deserializer.ReadNext(nullptr);
			}

			deserializer.rootJson = nullptr; // Unset json to prevent deletion

			return true;
		}
		else if (jsonValue->IsNullValue())
		{
			return true;
		}

		return false;
	}

} // namespace CE
