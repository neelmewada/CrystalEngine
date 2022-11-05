#pragma once

#include "Types/CoreTypeDefs.h"

#include "RTTI.h"

#include <vector>

namespace CE
{

    class CORE_API FieldType : public TypeInfo
    {
    private:
        FieldType(String name, TypeId fieldTypeId, SIZE_T size, SIZE_T offset, const char* attributes) : TypeInfo(name)
            , FieldTypeId(fieldTypeId), Attributes(attributes)
            , Size(size), Offset(offset)
        {}

    public:

        virtual bool IsField() const override { return true; }

        virtual TypeId GetTypeId() const
        {
            return FieldTypeId;
        }

        const char* GetRawAttributes() const { return Attributes; }

        inline SIZE_T GetOffset() const { return Offset; }
        inline SIZE_T GetSize() const { return Size; }

        // TypeData is always NULL for field types
        //virtual const u8* GetRawTypeData() const { return nullptr; }

    private:
        TypeId FieldTypeId;
        const char* Attributes;

        SIZE_T Offset;
        SIZE_T Size;

        friend class StructType;
        friend class ClassType;
    };
    
} // namespace CE

