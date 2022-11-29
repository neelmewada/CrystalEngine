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
        EnumConstant(CE::Name name, CE::TypeId typeId, s64 value, const char* attributes = "");

        template<typename T>
        friend const TypeInfo* GetStaticType();

    public:

        virtual bool IsEnumConstant() const { return true; }

        virtual TypeId GetTypeId() const override { return this->TypeId; }

        inline s64 GetValue() const { return Value; }

    private:
        TypeId TypeId;
        s64 Value;

        friend class EnumType;
    };

    class CORE_API EnumType : public TypeInfo
    {
    private:
        EnumType(CE::Name name, CE::TypeId typeId, std::initializer_list<EnumConstant> constants, const char* attributes = "");
        ~EnumType() = default;

        template<typename T>
        friend const TypeInfo* GetStaticType();

    public:

        virtual bool IsEnum() const override { return true; }

        virtual TypeId GetTypeId() const override { return this->TypeId; }

        inline u32 GetConstantsCount() const { return Constants.GetSize(); }
        inline const EnumConstant* GetConstant(u32 index) const { return &Constants[index]; }

    private:
        TypeId TypeId;
        CE::Array<EnumConstant> Constants{};
    };
    
} // namespace name

#define CE_ENUM_CONSTANTS(...) __VA_ARGS__
#define CE_CONST(Name, ...) CE::EnumConstant(#Name, CE::GetTypeId<Self>(), (CE::s64)Self::Name, "" #__VA_ARGS__)

#define CE_RTTI_ENUM(API, Namespace, Enum, Attributes, ...)\
namespace CE\
{\
    template<>\
    inline const TypeInfo* GetStaticType<Namespace::Enum>()\
    {\
        using Self = Namespace::Enum;\
        static EnumType instance = EnumType{ #Namespace "::" #Enum, CE::GetTypeId<Namespace::Enum>(), { __VA_ARGS__ }, #Attributes "" };\
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



