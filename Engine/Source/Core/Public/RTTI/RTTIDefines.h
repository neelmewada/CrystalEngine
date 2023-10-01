#pragma once

#include "Templates/TypeTraits.h"

#define TYPEID(type) CE::GetTypeId<type>()

#define TYPENAME(type) CE::GetTypeName<type>()

#define TYPE(type) CE::GetStaticType<type>()

#define CE_RTTI_POD(API, Namespace, Type, ...)\
namespace CE::Internal\
{\
	template<>\
	struct TypeInfoImpl<Namespace::Type> : public CE::TypeInfo\
	{\
	private:\
		TypeInfoImpl<Namespace::Type>() : TypeInfo(#Namespace "::" #Type)\
		{}\
	public:\
		API static const CE::Name& FullTypeName();\
		API static CE::TypeInfo* StaticType();\
		const CE::Name& GetTypeName() override { return FullTypeName(); }\
		virtual CE::TypeId GetTypeId() const override { return TYPEID(Namespace::Type); }\
		virtual bool IsPOD() const override { return true; }\
		virtual void InitializeDefaults(void* instance) override { new(instance) Namespace::Type(); }\
		virtual void CallDestructor(void* instance) override { CE::TTypeDestructor<Namespace::Type>::Invoke(instance); }\
		virtual void CopyConstructor(void* source, void* destination) override\
		{\
			*(Namespace::Type*)destination = *(Namespace::Type*)source;\
		}\
		virtual bool IsAssignableTo(TypeId typeId) override\
		{\
			std::initializer_list<TypeId> types = { __VA_ARGS__ };\
			for (auto assignableType : types)\
			{\
				if (typeId == assignableType)\
				{\
					return true;\
				}\
			}\
			return typeId == this->GetTypeId();\
		}\
		virtual u32 GetSize() const override\
		{\
			return (u32)sizeof(Namespace::Type);\
		}\
	};\
}\
namespace CE\
{\
	template<>\
	inline TypeInfo* GetStaticType<Namespace::Type>()\
	{\
        return CE::Internal::TypeInfoImpl<Namespace::Type>::StaticType();\
	}\
	template<>\
	inline CE::Name GetTypeName<Namespace::Type>()\
	{\
		return CE::Internal::TypeInfoImpl<Namespace::Type>::FullTypeName();\
	}\
}

#define CE_RTTI_POD_IMPL(Namespace, Type)\
CE::TypeInfo* CE::Internal::TypeInfoImpl<Namespace::Type>::StaticType()\
{\
	static CE::Internal::TypeInfoImpl<Namespace::Type> instance{};\
	return &instance;\
}\
const CE::Name& CE::Internal::TypeInfoImpl<Namespace::Type>::FullTypeName()\
{\
	static Name name = MAKE_NAME(PACKAGE_NAME, Namespace, Type);\
	return name;\
}

#define CE_RTTI_POD_TEMPLATE(API, Namespace, Type, DefaultArgType, ...)\
namespace CE::Internal\
{\
	template<>\
	struct TypeInfoImpl<Namespace::Type<DefaultArgType>> : public CE::TypeInfo\
	{\
	private:\
		TypeInfoImpl() : TypeInfo(#Namespace "::" #Type)\
		{}\
	public:\
		API static const CE::Name& FullTypeName();\
		API static CE::TypeInfo* StaticType();\
		const CE::Name& GetTypeName() override { return FullTypeName(); }\
		virtual CE::TypeId GetTypeId() const override { return TYPEID(Namespace::Type<DefaultArgType>); }\
		virtual bool IsPOD() const override { return true; }\
		virtual bool IsTemplatedPOD() const { return true; }\
		virtual void InitializeDefaults(void* instance) override { new(instance) Namespace::Type<DefaultArgType>(); }\
		virtual void CallDestructor(void* instance) override { TTypeDestructor<Namespace::Type<DefaultArgType>>::Invoke(instance); }\
		virtual bool IsAssignableTo(TypeId typeId) override\
		{\
			std::initializer_list<TypeId> types = { __VA_ARGS__ };\
			for (auto assignableType : types)\
			{\
				if (typeId == assignableType)\
				{\
					return true;\
				}\
			}\
			return typeId == this->GetTypeId();\
		}\
		virtual u32 GetSize() const override\
		{\
			return (u32)sizeof(Namespace::Type<DefaultArgType>);\
		}\
	};\
}\
namespace CE\
{\
	template<>\
	inline TypeInfo* GetStaticType<Namespace::Type<DefaultArgType>>()\
	{\
        return CE::Internal::TypeInfoImpl<Namespace::Type<DefaultArgType>>::StaticType();\
	}\
	template<>\
	inline CE::Name GetTypeName<Namespace::Type<DefaultArgType>>()\
	{\
		return CE::Internal::TypeInfoImpl<Namespace::Type<DefaultArgType>>::FullTypeName();\
	}\
    template<typename T>\
    struct TemplateType<Namespace::Type<T>> : TTrueType\
    {\
        typedef DefaultArgType DefaultArg;\
        typedef Namespace::Type<DefaultArg> DefaultTemplate;\
    };\
}

#define CE_RTTI_POD_TEMPLATE_IMPL(Namespace, Type, DefaultArgType)\
CE::TypeInfo* CE::Internal::TypeInfoImpl<Namespace::Type<DefaultArgType>>::StaticType()\
{\
	static CE::Internal::TypeInfoImpl<Namespace::Type<DefaultArgType>> instance{};\
	return &instance;\
}\
const CE::Name& CE::Internal::TypeInfoImpl<Namespace::Type<DefaultArgType>>::FullTypeName()\
{\
	static Name name = MAKE_NAME(PACKAGE_NAME, Namespace, Type);\
	return name;\
}

#define CE_RTTI_TYPEINFO(API, Namespace, Type, ...)\
namespace CE::Internal\
{\
	template<>\
	struct TypeInfoImpl<Namespace::Type> : public CE::TypeInfo\
	{\
	private:\
		TypeInfoImpl<Namespace::Type>() : TypeInfo(#Namespace "::" #Type)\
		{}\
	public:\
		API static const CE::Name& FullTypeName();\
		API static CE::TypeInfo* StaticType();\
		const CE::Name& GetTypeName() override { return FullTypeName(); }\
		virtual CE::TypeId GetTypeId() const override { return TYPEID(Namespace::Type); }\
		virtual bool IsTypeInfo() const override { return true; }\
		virtual void InitializeDefaults(void* instance) override { }\
		virtual void CallDestructor(void* instance) override { }\
		virtual bool IsAssignableTo(TypeId typeId) override\
		{\
			std::initializer_list<TypeId> types = { __VA_ARGS__ };\
			for (auto assignableType : types)\
			{\
				if (typeId == assignableType)\
				{\
					return true;\
				}\
			}\
			return typeId == this->GetTypeId();\
		}\
		virtual u32 GetSize() const override\
		{\
			return (u32)sizeof(Namespace::Type);\
		}\
	};\
}\
namespace CE\
{\
	template<>\
	inline TypeInfo* GetStaticType<Namespace::Type>()\
	{\
        return CE::Internal::TypeInfoImpl<Namespace::Type>::StaticType();\
	}\
	template<>\
	inline CE::Name GetTypeName<Namespace::Type>()\
	{\
		return CE::Internal::TypeInfoImpl<Namespace::Type>::FullTypeName();\
	}\
}

#define CE_RTTI_TYPEINFO_IMPL(Namespace, Type)\
CE::TypeInfo* CE::Internal::TypeInfoImpl<Namespace::Type>::StaticType()\
{\
	static CE::Internal::TypeInfoImpl<Namespace::Type> instance{};\
	return &instance;\
}\
const CE::Name& CE::Internal::TypeInfoImpl<Namespace::Type>::FullTypeName()\
{\
	static Name name = MAKE_NAME(PACKAGE_NAME, Namespace, Type);\
	return name;\
}

#define CE_REGISTER_TYPES(...) CE::RegisterTypes<__VA_ARGS__>();
#define CE_DEREGISTER_TYPES(...) CE::DeregisterTypes<__VA_ARGS__>();

namespace CE
{
	// Forward Decls

	class Object;

	class ObjectMap;

    template<typename T>
    class Array;

	// **********************************************************
	// Type ID

	typedef SIZE_T TypeId;
	typedef TypeId TypeIdSize;

	namespace Internal
	{
		template<typename Derived, typename Base>
		inline static PtrDiff ComputePointerOffset()
		{
			Derived* derivedPtr = (Derived*)1;
			Base* basePtr = static_cast<Base*>(derivedPtr);
			return (IntPtr)basePtr - (IntPtr)derivedPtr;
		}
        
        CORE_API TypeId GetArrayTypeId();
        CORE_API TypeId GetObjectStoreTypeId();
		CORE_API TypeId GetSubClassTypeTypeId();
	}

	template<typename Type>
	TypeId GetTypeId()
	{
		constexpr const bool isVoid = std::is_void_v<Type>;
		constexpr const bool isPointer = std::is_pointer_v<Type>;
		constexpr const bool isArray = TIsArray<Type>::Value;
		constexpr const bool isObjectStore = IsObjectStoreType<Type>::Value;
		constexpr const bool isSubClassType = TIsSubClassType<Type>::Value;
		
		typedef CE::RemovePointerFromType<Type> Type0;
		typedef CE::RemoveConstVolatileFromType<Type0> FinalType;
		
		if constexpr (isVoid)
		{
			return 0;
		}
		else if constexpr (isObjectStore) // ObjectMap type
		{
            return Internal::GetObjectStoreTypeId();
		}
		else if constexpr (isArray) // array type
		{
            return Internal::GetArrayTypeId();
		}
		else if constexpr (isSubClassType) // SubClassType<SomeClass>
		{
			return Internal::GetSubClassTypeTypeId();
		}
		else if constexpr (isPointer) // a pointer type
		{
			if constexpr (std::is_same_v<char, FinalType>)
			{
				return (TypeId)typeid(char*).hash_code(); // special case: TYPEID(char) != TYPEID(char*)
			}
			else
			{
				return (TypeId)typeid(FinalType).hash_code(); // always ignore pointers for TypeId. Ex: TYPEID(Object) == TYPEID(Object*)
			}
		}
		else // a plain data type
		{
			return (TypeId)typeid(FinalType).hash_code();
		}
	}

	template<typename T>
	struct TGetUnderlyingTypeId
	{
		static inline TypeId Get()
		{
			typedef TGetUnderlyingType<T>::Type UnderlyingType;
			return TYPEID(UnderlyingType);
		}
	};

	template <auto Start, auto End, auto Inc, class F>
	constexpr void constexpr_for(F&& f)
	{
		if constexpr (Start < End)
		{
			f(std::integral_constant<decltype(Start), Start>());
			constexpr_for<Start + Inc, End, Inc>(f);
		}
	}

	template<typename... Args>
	struct TGetUnderlyingTypeIdPack
	{
		enum : SIZE_T { NumArgs = sizeof...(Args) };

		typedef std::tuple<Args...> Tuple;

		template <SIZE_T i>
		struct Arg
		{
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type Type;
			// the i-th argument is equivalent to the i-th tuple element of a tuple
			// composed of those arguments.
		};

		static inline std::vector<TypeId> Get()
		{
			std::vector<TypeId> allTypes = std::vector<TypeId>();
			allTypes.resize(NumArgs, 0);

			constexpr_for<(SIZE_T)0, NumArgs, (SIZE_T)1>([&](auto i)
				{
					typedef Arg<i>::Type ArgType;
					allTypes[i] = TGetUnderlyingTypeId<ArgType>::Get();
				});

			return allTypes;
		}
	};

    
} // namespace CE

