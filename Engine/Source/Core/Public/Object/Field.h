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

        virtual bool IsField() const override { return true; }

        inline FieldType* GetNext() const { return next; }

        virtual TypeId GetTypeId() const
        {
            return fieldTypeId;
        }

        inline SIZE_T GetOffset() const { return offset; }

        virtual u32 GetSize() const override { return size; }

        // TypeData is always NULL for field types
        //virtual const u8* GetRawTypeData() const { return nullptr; }

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

