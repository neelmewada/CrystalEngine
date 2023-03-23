#pragma once

#include "Types/CoreTypeDefs.h"

#include "RTTI.h"

#include <vector>

namespace CE
{

    class CORE_API FieldType : public TypeInfo
    {
    private:
        FieldType(String name, TypeId fieldTypeId, SIZE_T size, SIZE_T offset, String attributes, const TypeInfo* owner = nullptr) : TypeInfo(name, attributes)
            , fieldTypeId(fieldTypeId)
            , size(size), offset(offset)
            , owner(owner)
        {}

    public:

        virtual String GetDisplayName() const override;

        virtual bool IsField() const override { return true; }

        CE_INLINE FieldType* GetNext() const { return next; }

        virtual TypeId GetTypeId() const override
        {
            return fieldTypeId;
        }

        CE_INLINE SIZE_T GetOffset() const { return offset; }

        virtual u32 GetSize() const override { return (u32)size; }

        // TypeData is always NULL for field types
        //virtual const u8* GetRawTypeData() const { return nullptr; }
        
        virtual bool IsAssignableTo(TypeId typeId) const override;
        virtual bool IsObject() const override { return IsAssignableTo(TYPEID(Object)); }

        bool IsSerialized() const;
        bool IsHidden() const;
        
        const TypeInfo* GetDeclarationType() const;

        CE_INLINE TypeId GetDeclarationTypeId() const { return fieldTypeId; }

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

    private:

        TypeId fieldTypeId;

        SIZE_T offset;
        SIZE_T size;

        FieldType* next = nullptr;
        const TypeInfo* owner = nullptr;

        friend class StructType;
        friend class ClassType;
    };
    
} // namespace CE

