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
        EnumConstant(const CE::Name& name, const CE::Name& enumFullName, CE::TypeId typeId, s64 value, u32 size, const char* attributes = "");

        template<typename T>
        friend TypeInfo* GetStaticType();

		template<typename Enum>
		friend struct CE::Internal::TypeInfoImpl;

    public:

		virtual const CE::Name& GetTypeName() override;

        virtual String GetDisplayName() override;

        virtual bool IsEnumConstant() const { return true; }

        virtual TypeId GetTypeId() const override { return this->typeId; }

        CE_INLINE s64 GetValue() const { return value; }
        virtual u32 GetSize() const override { return size; }

    private:
		Name enumFullName;
		Name constantFullName;
        TypeId typeId;
        s64 value;
        u32 size;

        friend class EnumType;
    };

	enum EnumFlags
	{
		ENUM_NoFlags = 0,
		ENUM_FlagsEnum = BIT(0),
	};
	ENUM_CLASS_FLAGS(EnumFlags);

    class CORE_API EnumType : public TypeInfo
    {
    private:
        EnumType(CE::Name name, CE::TypeId typeId, CE::TypeId underlyingTypeId, std::initializer_list<EnumConstant> constants, u32 size, const char* attributes = "");
        ~EnumType();

        template<typename T>
        friend TypeInfo* GetStaticType();

        template<typename Enum>
        friend struct CE::Internal::TypeInfoImpl;

		void ConstructInternal();

    public:

        virtual String GetDisplayName() override;

        virtual bool IsEnum() const override { return true; }

        virtual TypeId GetTypeId() const override { return this->typeId; }

		bool HasAnyEnumFlags(EnumFlags flags) const
		{
			return (enumFlags & flags) != 0;
		}

		bool HasAllEnumFlags(EnumFlags flags) const
		{
			return (enumFlags & flags) == flags;
		}

        CE_INLINE u32 GetConstantsCount() { return constants.GetSize(); }
        CE_INLINE EnumConstant* GetConstant(u32 index) 
        { 
            if (index >= GetConstantsCount())
                return nullptr;
            return &constants[index]; 
        }

		EnumConstant* FindConstantWithName(const Name& constantName);
        EnumConstant* FindConstantWithValue(s64 value);
		s64 GetConstantValue(const Name& constantName);

		template<typename TEnum> requires TIsEnum<TEnum>::Value
		FORCE_INLINE TEnum GetConstantValue(const Name& constantName)
		{
			if (CE::GetStaticEnum<TEnum>() != this)
				return {};
			return (TEnum)GetConstantValue(constantName);
		}

        virtual u32 GetSize() const override { return size; }

        virtual CE::TypeId GetUnderlyingTypeId() const override { return underlyingTypeId; }

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

		EnumFlags enumFlags = ENUM_NoFlags;

        static CE::HashMap<TypeId, EnumType*> registeredEnumsById;
        static CE::HashMap<Name, EnumType*> registeredEnumsByName;
    };
    
} // namespace name

#define CE_ENUM_CONSTANTS(...) __VA_ARGS__
#define CE_CONST(Name, ...) CE::EnumConstant(#Name, FullTypeName(), CE::GetTypeId<Self>(), (s64)Self::Name, sizeof(Self), "" #__VA_ARGS__)

#define CE_RTTI_ENUM(API, Namespace, Enum, Attributes, ...)\
namespace CE\
{\
    namespace Internal\
    {\
        template<>\
		struct TypeInfoImpl<Namespace::Enum> : public EnumType\
        {\
            typedef Namespace::Enum Self;\
			API static const CE::Name& FullTypeName();\
			const CE::Name& GetTypeName() override { return FullTypeName(); }\
            TypeInfoImpl()\
                : EnumType(#Namespace "::" #Enum, CE::GetTypeId<Namespace::Enum>(), CE::GetTypeId<__underlying_type(Namespace::Enum)>(), { __VA_ARGS__ }, sizeof(Namespace::Enum), CE_TOSTRING(Attributes) "")\
            {\
            }\
			virtual ~TypeInfoImpl()\
			{\
			}\
        };\
    }\
    template<>\
    inline TypeInfo* GetStaticType<Namespace::Enum>()\
    {\
        typedef Namespace::Enum Self;\
        static CE::Internal::TypeInfoImpl<Namespace::Enum> instance{};\
	    return &instance;\
    }\
    template<>\
    inline EnumType* GetStaticEnum<Namespace::Enum>()\
    {\
        return (EnumType*)GetStaticType<Namespace::Enum>();\
    }\
    template<>\
	inline CE::Name GetTypeName<Namespace::Enum>()\
	{\
		return CE::Internal::TypeInfoImpl<Namespace::Enum>::FullTypeName();\
	}\
	template<>\
	inline SIZE_T GetHash<Namespace::Enum>(const Namespace::Enum& enumValue)\
	{\
		return (__underlying_type(Namespace::Enum))enumValue;\
	}\
}\
template <> struct fmt::formatter<Namespace::Enum> {\
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {\
        return ctx.end();\
    }\
    template <typename FormatContext>\
    auto format(const Namespace::Enum& value, FormatContext& ctx) const -> decltype(ctx.out()) {\
        auto enumType = CE::GetStaticEnum<Namespace::Enum>();\
        auto constant = enumType->FindConstantWithValue((s64)value);\
        if (constant == nullptr)\
        {\
            return fmt::format_to(ctx.out(), "{}", (s64)value);\
        }\
        return fmt::format_to(ctx.out(), "{}", constant->GetName());\
    }\
};

#define CE_RTTI_ENUM_IMPL(API, Namespace, Enum)\
const CE::Name& CE::Internal::TypeInfoImpl<Namespace::Enum>::FullTypeName()\
{\
	static CE::Name name = MAKE_NAME(PACKAGE_NAME, Namespace, Enum);\
	return name;\
}

