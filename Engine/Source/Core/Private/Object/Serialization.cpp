
#include "CoreMinimal.h"

namespace CE
{

	FieldSerializer::FieldSerializer(FieldType* fieldChain, void* instance) : rawInstance(instance)
	{
		fields.Clear();

		FieldType* field = fieldChain;

		while (field != nullptr)
		{
			fields.Add(field);
			field = fieldChain->GetNext();
		}
	}

	bool FieldSerializer::HasNext()
	{
		return fields.NonEmpty();
	}

	bool FieldSerializer::WriteNext(Stream* stream)
	{
		if (stream == nullptr || !stream->CanWrite() || rawInstance == nullptr)
			return false;

		FieldType* field = fields.Top();
		TypeId fieldTypeId = field->GetDeclarationTypeId();
		
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
			//const auto& array = field->GetFieldValue<Array<u8>>(rawInstance).GetSize();
			auto underlyingType = field->GetUnderlyingType();
			
		}

		fields.Pop();
		return true;
	}

	FieldType* FieldSerializer::GetNext()
	{
		if (!HasNext())
			return nullptr;

		return fields.Top();
	}

}
