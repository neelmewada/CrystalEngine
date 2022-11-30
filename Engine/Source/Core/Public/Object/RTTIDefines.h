#pragma once

#include "TypeTraits.h"
#include "Containers/Array.h"

#define TYPEID(type) CE::GetTypeId<type>()

#define CE_RTTI_POD(Namespace, Type)\
namespace Namespace\
{\
	class CE_##Type##_TypeInfo : public CE::TypeInfo\
	{\
	private:\
		friend const TypeInfo* CE::GetStaticType<Namespace::Type>();\
		CE_##Type##_TypeInfo() : TypeInfo(#Namespace "::" #Type)\
		{\
			CE::TypeInfo::RegisterType(this);\
		}\
	public:\
		virtual CE::TypeId GetTypeId() const { return TYPEID(Namespace::Type); }\
		virtual bool IsPOD() const { return true; }\
	};\
}\
namespace CE\
{\
	template<>\
	inline const TypeInfo* GetStaticType<Namespace::Type>()\
	{\
		static Namespace::CE_##Type##_TypeInfo instance{};\
		return &instance;\
	}\
}



#define CE_RTTI_POD2(Type)\
class CE_##Type##_TypeInfo : public CE::TypeInfo\
{\
private:\
	friend const TypeInfo* CE::GetStaticType<Type>();\
	CE_##Type##_TypeInfo() : TypeInfo(#Type)\
	{\
		CE::TypeInfo::RegisterType(this);\
	}\
public:\
	virtual CE::TypeId GetTypeId() const { return TYPEID(Type); }\
	virtual bool IsPOD() const { return true; }\
};\
namespace CE\
{\
	template<>\
	inline const TypeInfo* GetStaticType<Type>()\
	{\
		static CE_##Type##_TypeInfo instance{};\
		return &instance;\
	}\
}

#define CE_REGISTER_TYPES(...) CE::RegisterTypes<__VA_ARGS__>();

namespace CE
{

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

		CORE_API extern TypeId GCurrentTypeId;

		struct CORE_API TypeIdGenerator
		{
			static TypeId Counter;
		};

		template<typename>
		struct ElementTypeId : TypeIdGenerator {
			static const TypeId Type;
		};

		template<typename T>
		const TypeId ElementTypeId<T>::Type = ElementTypeId::Counter++;
	}

	template<typename Type>
	TypeId GetTypeId()
	{
		constexpr const bool isPointer = std::is_pointer_v<Type>;
		
		typedef CE::RemovePointerFromType<Type> Type0;
		typedef CE::RemoveConstVolatileFromType<Type0> FinalType;

		if constexpr (isPointer) // a pointer type
		{
			return (TypeId)typeid(FinalType*).hash_code();
		}
		else // a plain data type
		{
			return (TypeId)typeid(FinalType).hash_code();
		}
	}
    
} // namespace CE

