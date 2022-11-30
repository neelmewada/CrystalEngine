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
            , FieldTypeId(fieldTypeId)
            , Size(size), Offset(offset)
            , Owner(owner)
        {}

    public:

        virtual bool IsField() const override { return true; }

        inline FieldType* GetNext() const { return Next; }

        virtual TypeId GetTypeId() const
        {
            return FieldTypeId;
        }

        inline SIZE_T GetOffset() const { return Offset; }
        inline SIZE_T GetSize() const { return Size; }

        // TypeData is always NULL for field types
        //virtual const u8* GetRawTypeData() const { return nullptr; }

    private:
        TypeId FieldTypeId;

        SIZE_T Offset;
        SIZE_T Size;

        FieldType* Next = nullptr;
        const TypeInfo* Owner = nullptr;

        friend class StructType;
        friend class ClassType;
    };
    
} // namespace CE

