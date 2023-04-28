#pragma once

#include "RTTI/RTTI.h"

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
    EnumType* GetStaticEnum()
    {
        return nullptr;
    }


    // Enum

    class CORE_API EnumConstant : public TypeInfo
    {
    private:
        EnumConstant(CE::Name name, CE::TypeId typeId, s64 value, u32 size, const char* attributes = "");

        template<typename T>
        friend TypeInfo* GetStaticType();

    public:

        virtual String GetDisplayName() override;

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
        ~EnumType();

        template<typename T>
        friend TypeInfo* GetStaticType();

        template<typename Enum>
        friend struct CE::Internal::TypeInfoImpl;

    public:

        virtual String GetDisplayName() override;

        virtual bool IsEnum() const override { return true; }

        virtual TypeId GetTypeId() const override { return this->typeId; }

        CE_INLINE u32 GetConstantsCount() { return constants.GetSize(); }
        CE_INLINE EnumConstant* GetConstant(u32 index) 
        { 
            if (index >= GetConstantsCount())
                return nullptr;
            return &constants[index]; 
        }

        EnumConstant* FindConstantWithValue(s64 value);

        virtual u32 GetSize() const override { return size; }

        CE_INLINE CE::TypeId GetUnderlyingTypeId() const { return underlyingTypeId; }

        auto GetFirstConstant() { return constants.Begin(); }
        auto GetLastConstant() { return constants.End(); }

        // For internal use only!
        static void RegisterEnumType(EnumType* enumType);
        static void DeregisterEnumType(EnumType* enumType);

        static EnumType* FindEnumByName(Name enumName);
        static EnumType* FindEnumById(TypeId enumId);

    private:
        TypeId underlyingTypeId;
        TypeId typeId;
        u32 size;
        CE::Array<EnumConstant> constants{};

        static CE::HashMap<TypeId, EnumType*> registeredEnumsById;
        static CE::HashMap<Name, EnumType*> registeredEnumsByName;
    };
    
} // namespace name

#define CE_ENUM_CONSTANTS(...) __VA_ARGS__
#define CE_CONST(Name, ...) CE::EnumConstant(#Name, CE::GetTypeId<Self>(), (CE::s64)Self::Name, sizeof(Self), "" #__VA_ARGS__)

#define CE_RTTI_ENUM(API, Namespace, Enum, Attributes, ...)\
namespace CE\
{\
    namespace Internal\
    {\
        template<>\
		struct TypeInfoImpl<Namespace::Enum>\
        {\
            typedef Namespace::Enum Self;\
            CE::EnumType enumType;\
            TypeInfoImpl(CE::EnumType&& enumTypeRef)\
                : enumType(enumTypeRef)\
            {\
                TypeInfo::RegisterType(&enumType);\
                EnumType::RegisterEnumType(&enumType);\
            }\
			virtual ~TypeInfoImpl()\
			{\
				TypeInfo::DeregisterType(&enumType);\
                EnumType::DeregisterEnumType(&enumType);\
			}\
        };\
    }\
    template<>\
    inline TypeInfo* GetStaticType<Namespace::Enum>()\
    {\
        typedef Namespace::Enum Self;\
        static Internal::TypeInfoImpl<Namespace::Enum> instance{ EnumType{ #Namespace "::" #Enum, CE::GetTypeId<Namespace::Enum>(), CE::GetTypeId<__underlying_type(Namespace::Enum)>(), { __VA_ARGS__ }, sizeof(Namespace::Enum), CE_TOSTRING(Attributes) "" } };\
	    return &instance.enumType;\
    }\
    template<>\
    inline EnumType* GetStaticEnum<Namespace::Enum>()\
    {\
        return (EnumType*)GetStaticType<Namespace::Enum>();\
    }\
    template<>\
	inline CE::Name GetTypeName<Namespace::Enum>()\
	{\
		static Name name = Name(#Namespace "::" #Enum);\
		return name;\
	}\
}\
template <> struct fmt::formatter<Namespace::Enum> {\
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {\
        return ctx.end();\
    }\
    template <typename FormatContext>\
    auto format(const Namespace::Enum& value, FormatContext& ctx) const -> decltype(ctx.out()) {\
        auto enumType = CE::GetStaticEnum<Namespace::Enum>();\
        auto constant = enumType->FindConstantWithValue((CE::s64)value);\
        if (constant == nullptr)\
        {\
            return fmt::format_to(ctx.out(), "{}", (CE::s64)value);\
        }\
        return fmt::format_to(ctx.out(), "{}", constant->GetName());\
    }\
};

#define CE_RTTI_ENUM_IMPL(API, Namespace, Enum)

