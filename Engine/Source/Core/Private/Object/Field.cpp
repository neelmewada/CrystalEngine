
#include "CoreMinimal.h"

namespace CE
{
    void FieldType::ConstructInternal()
    {
        fieldFlags = FIELD_NoFlags;

        if (!HasAttribute("NonSerialized"))
            fieldFlags |= FIELD_Serializable;
        if (HasAttribute("Hidden"))
            fieldFlags |= FIELD_Hidden;
        if (HasAttribute("Config"))
            fieldFlags |= FIELD_Config;
        if (HasAttribute("ReadOnly"))
            fieldFlags |= FIELD_ReadOnly;
    }

    const CE::Name& FieldType::GetTypeName()
    {
		if (!typeName.IsValid())
		{
			if (owner != nullptr)
				typeName = owner->GetTypeName().GetString() + "::" + name.GetString();
			else
				typeName = name;
		}
		return typeName;
    }

    String FieldType::GetDisplayName()
    {
        String displayAttrib = GetAttribute("Display").GetStringValue();

        if (!displayAttrib.IsEmpty())
            return displayAttrib;

        return TypeInfo::GetDisplayName();
    }

    bool FieldType::IsAssignableTo(TypeId typeId)
    {
        const auto declType = GetDeclarationType();
        
        if (declType == nullptr)
            return false;
        
        if (declType->IsAssignableTo(typeId))
            return true;
        
        return false;
    }

    bool FieldType::IsSerialized() const
    {
        return fieldFlags & FIELD_Serializable;
    }

    bool FieldType::IsHidden() const
    {
        return fieldFlags & FIELD_Hidden;
    }

    bool FieldType::IsReadOnly() const
    {
        return fieldFlags & FIELD_ReadOnly;
    }

	TypeInfo* FieldType::GetOwnerType()
	{
		return const_cast<TypeInfo*>(owner);
	}

	TypeInfo* FieldType::GetInstanceOwnerType()
	{
		return const_cast<TypeInfo*>(instanceOwner);
	}

    bool FieldType::IsArrayField() const
    {
        return GetDeclarationTypeId() == TYPEID(Array<u8>);
    }

    bool FieldType::IsStringField() const
    {
        return GetDeclarationTypeId() == TYPEID(String);
    }

    bool FieldType::IsIntegerField() const
    {
        return fieldTypeId == TYPEID(u8) || fieldTypeId == TYPEID(u16) || fieldTypeId == TYPEID(u32) || fieldTypeId == TYPEID(u64) ||
            fieldTypeId == TYPEID(s8) || fieldTypeId == TYPEID(s16) || fieldTypeId == TYPEID(s32) || fieldTypeId == TYPEID(s64);
    }

    bool FieldType::IsDecimalField() const
    {
        return fieldTypeId == TYPEID(f32) || fieldTypeId == TYPEID(f64);
    }

    bool FieldType::IsObjectField() const
    {
        auto classType = ClassType::FindClass(fieldTypeId);
        return classType != nullptr && classType->IsObject();
    }

	TypeInfo* FieldType::GetUnderlyingType()
	{
		if (underlyingTypeInfo == nullptr && underlyingTypeId != 0)
		{
			underlyingTypeInfo = GetTypeInfo(underlyingTypeId);
		}
		return underlyingTypeInfo;
	}

	TypeInfo* FieldType::GetDeclarationType()
    {
		if (declarationType == nullptr)
		{
			declarationType = GetTypeInfo(fieldTypeId);
		}
		return declarationType;
    }

    s64 FieldType::GetFieldEnumValue(void* instance)
    {
        if (!GetDeclarationType()->IsEnum())
            return 0;

        auto enumType = GetDeclarationType();

        if (enumType->GetSize() == sizeof(u8))
        {
            return GetFieldValue<u8>(instance);
        }
        else if (enumType->GetSize() == sizeof(u16))
        {
            return GetFieldValue<u16>(instance);
        }
        else if (enumType->GetSize() == sizeof(u32))
        {
            return GetFieldValue<u32>(instance);
        }
        else if (enumType->GetSize() == sizeof(u64))
        {
            return GetFieldValue<u64>(instance);
        }

        return 0;
    }

    void FieldType::SetFieldEnumValue(void* instance, s64 value)
    {
        if (!GetDeclarationType()->IsEnum())
            return;
		if (IsReadOnly())
			return;

		ForceSetFieldEnumValue(instance, value);
    }

	void FieldType::ForceSetFieldEnumValue(void* instance, s64 value)
	{
		auto enumType = GetDeclarationType();

		if (enumType->GetSize() == sizeof(u8))
		{
			SetFieldValue<u8>(instance, value);
		}
		else if (enumType->GetSize() == sizeof(u16))
		{
			SetFieldValue<u16>(instance, value);
		}
		else if (enumType->GetSize() == sizeof(u32))
		{
			SetFieldValue<u32>(instance, value);
		}
		else if (enumType->GetSize() == sizeof(u64))
		{
			SetFieldValue<u64>(instance, value);
		}
	}

	u32 FieldType::GetArraySize(void* instance)
	{
		if (!IsArrayField())
			return 0;

		const auto& array = GetFieldValue<Array<u8>>(instance);
		TypeId underlyingTypeId = GetUnderlyingTypeId();
		if (underlyingTypeId == 0)
			return 0;

		TypeInfo* underlyingType = GetUnderlyingType();
		if (underlyingType == nullptr)
			return 0;
		auto underlyingTypeSize = underlyingType->GetSize();

		if (underlyingType->IsClass())
		{
			underlyingTypeSize = sizeof(Object*);
		}

		return array.GetSize() / underlyingTypeSize;
	}

	void FieldType::ResizeArray(void* instance, u32 numElements)
	{
		if (!IsArrayField())
			return;

		auto& array = GetFieldValue<Array<u8>>(instance);
		TypeId underlyingTypeId = GetUnderlyingTypeId();
		if (underlyingTypeId == 0)
			return;

		TypeInfo* underlyingType = GetUnderlyingType();
		if (underlyingType == nullptr)
			return;

		auto underlyingTypeSize = underlyingType->GetSize();

		if (underlyingType->IsClass())
		{
			underlyingTypeSize = sizeof(Object*);
		}

		array.Resize(numElements * underlyingTypeSize);
	}

	Array<FieldType> FieldType::GetArrayFieldList(void* instance)
	{
		if (!IsArrayField())
			return {};
		
		u32 arraySize = GetArraySize(instance);
		if (arraySize == 0)
			return {};

		TypeInfo* underlyingType = GetUnderlyingType();
		if (underlyingType == nullptr)
			return {};

		auto underlyingTypeSize = underlyingType->GetSize();

		if (underlyingType->IsClass())
		{
			underlyingTypeSize = sizeof(Object*);
		}

		Array<FieldType> array{};

		for (int i = 0; i < arraySize; i++)
		{
			array.Add(FieldType(name.GetString() + "_" + i, underlyingType->GetTypeId(), 0, underlyingTypeSize, i * underlyingTypeSize, "", this));
		}

		return array;
	}

}
