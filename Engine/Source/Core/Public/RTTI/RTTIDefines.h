#pragma once

#include "Templates/TypeTraits.h"
#include "Types/Name.h"

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
		API static CE::TypeInfo* StaticType();\
		virtual CE::TypeId GetTypeId() const override { return TYPEID(Namespace::Type); }\
		virtual bool IsPOD() const override { return true; }\
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
		static Name name = Name(#Namespace "::" #Type);\
		return name;\
	}\
}

#define CE_RTTI_POD_IMPL(Namespace, Type)\
CE::TypeInfo* CE::Internal::TypeInfoImpl<Namespace::Type>::StaticType()\
{\
	static CE::Internal::TypeInfoImpl<Namespace::Type> instance{};\
	return &instance;\
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
		API static CE::TypeInfo* StaticType();\
		virtual CE::TypeId GetTypeId() const override { return TYPEID(Namespace::Type<DefaultArgType>); }\
		virtual bool IsPOD() const override { return true; }\
		virtual bool IsTemplatedPOD() const { return true; }\
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
		static Name name = Name(#Namespace "::" #Type);\
		return name;\
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
	}

	template<typename Type>
	TypeId GetTypeId()
	{
		constexpr const bool isVoid = std::is_void_v<Type>;
		constexpr const bool isPointer = std::is_pointer_v<Type>;
		constexpr const bool isArray = TIsArray<Type>::Value;
		constexpr const bool isObjectStore = IsObjectStoreType<Type>::Value;
		
		typedef CE::RemovePointerFromType<Type> Type0;
		typedef CE::RemoveConstVolatileFromType<Type0> FinalType;
		
		if constexpr (isVoid)
		{
			return 0;
		}
		else if constexpr (isObjectStore) // object store type
		{
            return Internal::GetObjectStoreTypeId();
		}
		else if constexpr (isArray) // array type
		{
            return Internal::GetArrayTypeId();
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
    
} // namespace CE

