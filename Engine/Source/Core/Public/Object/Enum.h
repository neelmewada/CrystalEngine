#pragma once

#include "RTTI.h"

#include "Containers/Array.h"

namespace CE
{
    // Forward declaration

    class CORE_API Name;
    class CORE_API EnumType;
    class CORE_API EnumConstant;

    // *************************************************
    // RTTI

    // Default implementation always returns nullptr. Specialization will return the correct data
    template<typename Type>
    const EnumType* GetStaticEnum()
    {
        return nullptr;
    }


    // Enum

    class CORE_API EnumConstant : public TypeInfo
    {
    private:
        EnumConstant(CE::Name name, CE::TypeId typeId, s64 value, u32 size, const char* attributes = "");

        template<typename T>
        friend const TypeInfo* GetStaticType();

    public:

        virtual bool IsEnumConstant() const { return true; }

        virtual TypeId GetTypeId() const override { return this->typeId; }

        CE_INLINE s64 GetValue() const { return value; }
        virtual u32 GetSize() const override { return size; }

    private:
        TypeId typeId;
        s64 value;
        u32 size;

        friend class EnumType;
    };

    class CORE_API EnumType : public TypeInfo
    {
    private:
        EnumType(CE::Name name, CE::TypeId typeId, CE::TypeId underlyingTypeId, std::initializer_list<EnumConstant> constants, u32 size, const char* attributes = "");
        ~EnumType() = default;

        template<typename T>
        friend const TypeInfo* GetStaticType();

    public:

        virtual bool IsEnum() const override { return true; }

        virtual TypeId GetTypeId() const override { return this->typeId; }

        CE_INLINE u32 GetConstantsCount() const { return constants.GetSize(); }
        CE_INLINE const EnumConstant* GetConstant(u32 index) const { return &constants[index]; }

        virtual u32 GetSize() const override { return size; }

        CE_INLINE CE::TypeId GetUnderlyingTypeId() const { return underlyingTypeId; }

    private:
        TypeId underlyingTypeId;
        TypeId typeId;
        u32 size;
        CE::Array<EnumConstant> constants{};
    };
    
} // namespace name

#define CE_ENUM_CONSTANTS(...) __VA_ARGS__
#define CE_CONST(Name, ...) CE::EnumConstant(#Name, CE::GetTypeId<Self>(), (CE::s64)Self::Name, sizeof(Self), "" #__VA_ARGS__)

#define CE_RTTI_ENUM(API, Namespace, Enum, Attributes, ...)\
namespace CE\
{\
    template<>\
    inline const TypeInfo* GetStaticType<Namespace::Enum>()\
    {\
        using Self = Namespace::Enum;\
        static EnumType instance = EnumType{ #Namespace "::" #Enum, CE::GetTypeId<Namespace::Enum>(), CE::GetTypeId<__underlying_type(Namespace::Enum)>(), { __VA_ARGS__ }, sizeof(Namespace::Enum), #Attributes "" };\
	    return &instance;\
    }\
    template<>\
    inline const EnumType* GetStaticEnum<Namespace::Enum>()\
    {\
        return (EnumType*)GetStaticType<Namespace::Enum>();\
    }\
}\
namespace Namespace\
{\
    API extern const TypeInfo* CE_Generated_EnumType_##Enum##_Registrant;\
}

#define CE_RTTI_ENUM_IMPL(API, Namespace, Enum)\
API const TypeInfo* Namespace::CE_Generated_EnumType_##Enum##_Registrant = CE::GetStaticEnum<Namespace::Enum>();



