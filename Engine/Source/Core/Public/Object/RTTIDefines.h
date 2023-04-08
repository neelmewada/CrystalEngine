#pragma once

#include "TypeTraits.h"

#define TYPEID(type) CE::GetTypeId<type>()

#define CE_RTTI_POD(Namespace, Type, ...)\
namespace Namespace\
{\
	class CE_##Type##_TypeInfo : public CE::TypeInfo\
	{\
	private:\
		friend TypeInfo* CE::GetStaticType<Namespace::Type>();\
		CE_##Type##_TypeInfo() : TypeInfo(#Namespace "::" #Type)\
		{\
			CE::TypeInfo::RegisterType(this);\
		}\
	public:\
		virtual CE::TypeId GetTypeId() const { return TYPEID(Namespace::Type); }\
		virtual bool IsPOD() const { return true; }\
		virtual bool IsAssignableTo(TypeId typeId) const override\
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
		virtual CE::u32 GetSize() const override\
		{\
			return (CE::u32)sizeof(Namespace::Type);\
		}\
	};\
}\
namespace CE\
{\
	template<>\
	inline TypeInfo* GetStaticType<Namespace::Type>()\
	{\
		static Namespace::CE_##Type##_TypeInfo instance{};\
		return &instance;\
	}\
}



#define CE_RTTI_POD2(Type, ...)\
class CE_##Type##_TypeInfo : public CE::TypeInfo\
{\
private:\
	friend TypeInfo* CE::GetStaticType<Type>();\
	CE_##Type##_TypeInfo() : TypeInfo(#Type)\
	{\
		CE::TypeInfo::RegisterType(this);\
	}\
public:\
	virtual CE::TypeId GetTypeId() const { return TYPEID(Type); }\
	virtual bool IsPOD() const { return true; }\
	virtual bool IsAssignableTo(CE::TypeId typeId) const override\
	{\
		std::initializer_list<CE::TypeId> types = { __VA_ARGS__ };\
		for (auto assignableType : types)\
		{\
			if (typeId == assignableType)\
			{\
				return true;\
			}\
		}\
		return typeId == this->GetTypeId();\
	}\
	virtual CE::u32 GetSize() const override\
	{\
		return (CE::u32)sizeof(Type);\
	}\
};\
namespace CE\
{\
	template<>\
	inline TypeInfo* GetStaticType<Type>()\
	{\
		static CE_##Type##_TypeInfo instance{};\
		return &instance;\
	}\
}

#define CE_RTTI_POD3(Namespace, Type, ActualType, ...)\
namespace Namespace\
{\
    class CE_##Type##_TypeInfo : public CE::TypeInfo\
    {\
    private:\
        friend TypeInfo* CE::GetStaticType<Namespace::ActualType>();\
        CE_##Type##_TypeInfo() : TypeInfo(#Namespace "::" #Type)\
        {\
            CE::TypeInfo::RegisterType(this);\
        }\
    public:\
        virtual CE::TypeId GetTypeId() const { return TYPEID(Namespace::ActualType); }\
        virtual bool IsPOD() const { return true; }\
        virtual bool IsAssignableTo(TypeId typeId) const override\
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
        virtual CE::u32 GetSize() const override\
        {\
            return (CE::u32)sizeof(Namespace::ActualType);\
        }\
    };\
}\
namespace CE\
{\
    template<>\
    inline TypeInfo* GetStaticType<Namespace::ActualType>()\
    {\
        static Namespace::CE_##Type##_TypeInfo instance{};\
        return &instance;\
    }\
}

#define CE_REGISTER_TYPES(...) CE::RegisterTypes<__VA_ARGS__>();

namespace CE
{
	// Forward Decls

	class Object;

	class ObjectStore;

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
		constexpr const bool isPointer = std::is_pointer_v<Type>;
		constexpr const bool isArray = IsArrayType<Type>::value;
		constexpr const bool isObjectStore = IsObjectStoreType<Type>::value;
		
		typedef CE::RemovePointerFromType<Type> Type0;
		typedef CE::RemoveConstVolatileFromType<Type0> FinalType;
		
		if constexpr (isObjectStore) // object store type
		{
            return Internal::GetObjectStoreTypeId();
		}
		else if constexpr (isArray) // array type
		{
            return Internal::GetArrayTypeId();
		}
		else if constexpr (isPointer) // a pointer type
		{
			return (TypeId)typeid(FinalType).hash_code(); // always ignore pointers for TypeId. i.e. TYPEID(Object) = TYPEID(Object*)
		}
		else // a plain data type
		{
			return (TypeId)typeid(FinalType).hash_code();
		}
	}
    
} // namespace CE

