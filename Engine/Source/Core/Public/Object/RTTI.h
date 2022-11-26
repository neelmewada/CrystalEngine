#pragma once

#include "Misc/CoreDefines.h"
#include "Misc/CoreMacros.h"
#include "Containers/Array.h"
#include "Containers/HashMap.h"
#include "Containers/String.h"

#include "TypeTraits.h"


namespace CE
{
	// Forward Decls

	template<typename ElementType>
	class Array;

    namespace Internal
    {
        template<typename Struct>
        struct TypeInfoImpl;
    }

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
        
        template<typename Struct>
        friend struct TypeInfoImpl;
        
        friend const TypeInfo* GetStaticType(String namePath);

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
        static HashMap<String, const TypeInfo*> RegisteredTypes;
        
    public:
        // For internal use only!
        static void RegisterType(const TypeInfo* type);
        
        inline static u32 GetRegisteredCount()
        {
            return RegisteredTypes.GetSize();
        }
	};

	// Default implementation always returns nullptr. Specialization will return the correct data
	template<typename Type>
	const TypeInfo* GetStaticType()
	{
		return nullptr;
	}

    CORE_API const TypeInfo* GetStaticType(String namePath);

	// Specialization will contain the magic data.
	template<typename T>
	struct StructTypeData
	{};


} // namespace CE


