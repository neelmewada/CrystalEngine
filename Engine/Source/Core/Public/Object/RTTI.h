#pragma once

#include "Misc/CoreDefines.h"
#include "Misc/CoreMacros.h"
#include "Containers/Array.h"
#include "Containers/HashMap.h"
#include "Containers/String.h"

#include "Types/CoreTypeDefs.h"

#include "RTTIDefines.h"

#include "Types/Name.h"

#include <iostream>


namespace CE
{
	// Forward Decls

	template<typename ElementType>
	class Array;

	class CORE_API Name;

	namespace Internal
	{
		template<typename Struct>
		struct TypeInfoImpl;
	}

	// **********************************************************
	// Type Info

	struct CORE_API TypeInfo
	{
	protected:
		TypeInfo(CE::Name name, CE::String attributes = "") : name(name)
		{}
        
        template<typename Struct>
        friend struct TypeInfoImpl;

	public:
		const CE::Name& GetName() const { return name; }
		const CE::String& GetAttributes() const { return attributes; }

		// TypeData is always located AFTER TypeInfo in memory
		//virtual const u8* GetRawTypeData() const { return (u8*)(this + 1); }

		virtual bool IsClass() const { return false; }
		virtual bool IsStruct() const { return false; }
		virtual bool IsField() const { return false; }
		virtual bool IsFunction() const { return false; }
		virtual bool IsEnum() const { return false; }
		virtual bool IsEnumConstant() const { return false; }

		virtual bool IsPOD() const { return false; }

		virtual bool IsAssignableTo(TypeId typeId) const { return false; }

		virtual TypeId GetTypeId() const = 0;
		virtual u32 GetSize() const = 0;

		virtual bool IsArrayType() const { return this->GetTypeId() == TYPEID(Array<u8>); }
        virtual bool IsObjectStoreType() const { return this->GetTypeId() == TYPEID(ObjectStore<Object>); }

	private:
		CE::Name name;
		CE::String attributes;

	public:
		// For internal use only!
        static HashMap<CE::Name, const TypeInfo*> RegisteredTypes;
		// For internal use only!
		static HashMap<TypeId, const TypeInfo*> RegisteredTypeIds;

        // For internal use only!
        static void RegisterType(const TypeInfo* type);
		// For internal use only!
        CE_INLINE static u32 GetRegisteredCount()
        {
            return RegisteredTypes.GetSize();
        }
	};

	/// Default implementation always returns nullptr. Specialization will return the correct data.
	/// Returns the type info of the specified type at compile time.
	template<typename Type>
	const TypeInfo* GetStaticType()
	{
		return nullptr;
	}

	/// Returns dynamic type info of the type with specified name. Ex: ParentNamespace::ChildNamespace::SomeClass
    CORE_API const TypeInfo* GetTypeInfo(CE::Name name);

	/// Returns dynamic type info of the type with specified type id.
	CORE_API const TypeInfo* GetTypeInfo(TypeId typeId);

	// Specialization will contain the magic data.
	template<typename T>
	struct StructTypeData
	{};

	template<typename... Args>
    CE_INLINE void RegisterTypes()
	{
		const TypeInfo* types[] = { GetStaticType<Args>()... };
	}

} // namespace CE

// Simple data types

CE_RTTI_POD2(bool, TYPEID(CE::s8), TYPEID(CE::u8))

CE_RTTI_POD(CE, s8,  TYPEID(s16), TYPEID(s32), TYPEID(s64), TYPEID(u8), TYPEID(u16), TYPEID(u32), TYPEID(u64))
CE_RTTI_POD(CE, s16, TYPEID(s8),  TYPEID(s32), TYPEID(s64), TYPEID(u8), TYPEID(u16), TYPEID(u32), TYPEID(u64))
CE_RTTI_POD(CE, s32, TYPEID(s8),  TYPEID(s16), TYPEID(s64), TYPEID(u8), TYPEID(u16), TYPEID(u32), TYPEID(u64))
CE_RTTI_POD(CE, s64, TYPEID(s8),  TYPEID(s16), TYPEID(s32), TYPEID(u8), TYPEID(u16), TYPEID(u32), TYPEID(u64))

CE_RTTI_POD(CE, u8,  TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64), TYPEID(u16), TYPEID(u32), TYPEID(u64))
CE_RTTI_POD(CE, u16, TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64), TYPEID(u8),  TYPEID(u32), TYPEID(u64))
CE_RTTI_POD(CE, u32, TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64), TYPEID(u8),  TYPEID(u16), TYPEID(u64))
CE_RTTI_POD(CE, u64, TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64), TYPEID(u8),  TYPEID(u16), TYPEID(u32))

CE_RTTI_POD(CE, f32, TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64), TYPEID(u8),  TYPEID(u16), TYPEID(u32), TYPEID(u64), TYPEID(f64))
CE_RTTI_POD(CE, f64, TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64), TYPEID(u8),  TYPEID(u16), TYPEID(u32), TYPEID(u64), TYPEID(f32))

CE_RTTI_POD(CE, String, TYPEID(const char*), TYPEID(char*))

