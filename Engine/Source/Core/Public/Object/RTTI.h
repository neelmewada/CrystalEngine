#pragma once

#include "Misc/CoreDefines.h"
#include "Misc/CoreMacros.h"
#include "Containers/Array.h"
#include "Containers/String.h"

#include "TypeTraits.h"


namespace CE
{
	// Forward Decls

	template<typename ElementType>
	class Array;

	// **********************************************************
	// Type ID

	typedef u32 TypeId;
	typedef TypeId TypeIdSize;

	namespace Internal
	{
		inline TypeId GenerateNewTypeId()
		{
			static TypeId id = 0;
			return ++id;
		}

		template<typename Derived, typename Base>
		inline static PtrDiff ComputePointerOffset()
		{
			Derived* derivedPtr = (Derived*)1;
			Base* basePtr = static_cast<Base*>(derivedPtr);
			return (IntPtr)basePtr - (IntPtr)derivedPtr;
		}

		template<typename Type>
		TypeId GetTypeIdInternal()
		{
			static TypeId typeId = Internal::GenerateNewTypeId();
			return typeId;
		}

		
	}

	template<typename Type>
	inline TypeId GetTypeId()
	{
		constexpr bool isPointer = std::is_pointer<Type>::value;

        typedef typename CE::RemovePointerFromType<Type> TypeWithoutPtr;
		typedef typename CE::RemoveConstVolatileFromType<TypeWithoutPtr> FinalType;

		if constexpr (isPointer)
		{
			return Internal::GetTypeIdInternal<FinalType*>();
		}
		else
		{
			return Internal::GetTypeIdInternal<FinalType>();
		}
	}

	// **********************************************************
	// Type Info

	struct TypeInfo
	{
	protected:
		TypeInfo(String name) : Name(name)
		{}

	public:
		const String& GetName() const { return Name; }

		// TypeData is always located AFTER TypeInfo in memory
		//virtual const u8* GetRawTypeData() const { return (u8*)(this + 1); }

		virtual bool IsClass() const { return false; }
		virtual bool IsStruct() const { return false; }
		virtual bool IsField() const { return false; }
		virtual bool IsFunction() const { return false; }

		virtual TypeId GetTypeId() const = 0;

	private:
		String Name;
	};

	// Default implementation always returns nullptr. Specialization will return the correct data
	template<typename Type>
	const TypeInfo* GetStaticType()
	{
		return nullptr;
	}

	// Specialization will contain the magic data.
	template<typename T>
	struct StructTypeData
	{};


} // namespace CE


