#pragma once

#include "Types/CoreTypeDefs.h"

#include "RTTI/RTTI.h"

#include <vector>

namespace CE
{

    enum FieldFlags
    {
        FIELD_NoFlags = 0,
        FIELD_Hidden = BIT(0),
        FIELD_ReadOnly = BIT(1),
        FIELD_Serializable = BIT(2),
        FIELD_Config = BIT(3)
    };
    ENUM_CLASS_FLAGS(FieldFlags);
    
    class CORE_API FieldType : public TypeInfo
    {
    private:
        FieldType(String name, TypeId fieldTypeId, TypeId underlyingTypeId, SIZE_T size, SIZE_T offset, String attributes, const TypeInfo* owner = nullptr) : TypeInfo(name, attributes)
            , fieldTypeId(fieldTypeId)
            , underlyingTypeId(underlyingTypeId)
            , size(size), offset(offset)
            , owner(owner)
        {
            ConstructInternal();
        }

        void ConstructInternal();

    public:

        String GetDisplayName() override;
        
        bool IsField() const override { return true; }

        INLINE FieldType* GetNext() const { return next; }

        TypeId GetTypeId() const override
        {
            return fieldTypeId;
        }

        INLINE SIZE_T GetOffset() const { return offset; }

        virtual u32 GetSize() const override { return (u32)size; }
        
        virtual bool IsAssignableTo(CE::TypeId typeId) override;
        virtual bool IsObject() override { return IsAssignableTo(TYPEID(Object)); }
        
        bool IsArrayField() const;
        bool IsStringField() const;

        bool IsIntegerField() const;
        bool IsDecimalField() const;

        bool IsObjectField() const;

        bool HasAnyFieldFlags(FieldFlags flags) const
        {
            return (fieldFlags & flags) != 0;
        }

        bool HasAllFieldFlags(FieldFlags flags) const
        {
            return (fieldFlags & flags) == flags;
        }

        bool IsSerialized() const;
        bool IsHidden() const;
        bool IsReadOnly() const;

		TypeInfo* GetOwnerType();
        
        virtual CE::TypeId GetUnderlyingTypeId() const override { return underlyingTypeId; }

		TypeInfo* GetUnderlyingType() override;
        
        TypeInfo* GetDeclarationType();

        INLINE TypeId GetDeclarationTypeId() const { return fieldTypeId; }

        template<typename T>
        T& GetFieldValue(void* instance)
        {
            return *(T*)((SIZE_T)instance + offset);
        }

        template<typename T>
        const T& GetFieldValue(void* instance) const
        {
            return *(T*)((SIZE_T)instance + offset);
        }
        
        template<typename T>
        void SetFieldValue(void* instance, const T& value)
        {
            *(T*)((SIZE_T)instance + offset) = value;
        }

        s64 GetFieldEnumValue(void* instance);
        void SetFieldEnumValue(void* instance, s64 value);

		// - Array API -

		u32 GetArraySize(void* instance);

		template<typename T>
		const T& GetArrayElementAt(u32 index, void* instance)
		{
			if (!IsArrayField())
				return {};

			const auto& array = GetFieldValue<Array<u8>>(instance);
			const u8* address = array.begin() + index;
			return *(T*)address;
		}

    private:
        FieldFlags fieldFlags = FIELD_NoFlags;
        
        TypeId fieldTypeId;
        TypeId underlyingTypeId;

		TypeInfo* declarationType = nullptr;
		TypeInfo* underlyingTypeInfo = nullptr;

        SIZE_T offset;
        SIZE_T size;

        FieldType* next = nullptr;
        const TypeInfo* owner = nullptr;

        friend class StructType;
        friend class ClassType;
    };
    
} // namespace CE

