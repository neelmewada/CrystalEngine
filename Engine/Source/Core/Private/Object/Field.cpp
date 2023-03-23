
#include "Object/Field.h"

namespace CE
{
    String FieldType::GetDisplayName() const
    {
        String displayAttrib = GetLocalAttributeValue("Display");

        if (!displayAttrib.IsEmpty())
            return displayAttrib;

        return TypeInfo::GetDisplayName();
    }

    bool FieldType::IsAssignableTo(TypeId typeId) const
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
        return !attributes.Exists([](const CE::Attribute& attr) -> bool
            {
                return attr.GetKey() == "NonSerialized";
            }
        );
    }

    bool FieldType::IsHidden() const
    {
        return attributes.Exists([](const CE::Attribute& attr) -> bool
            {
                return attr.GetKey() == "Hidden";
            }
        );
    }

    const TypeInfo* FieldType::GetDeclarationType() const
    {
        return GetTypeInfo(fieldTypeId);
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

}
