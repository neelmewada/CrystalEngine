#pragma once

#include "Misc/CoreDefines.h"
#include "Misc/CoreMacros.h"
#include "Containers/String.h"
#include "Containers/HashMap.h"
#include "RTTI.h"


namespace CE
{

	// Forward Declarations
	

	// **********************************************************
	// Variant

	class CORE_API VariantCastException : public std::runtime_error
	{
	public:
		VariantCastException(const char* message) : std::runtime_error(message)
		{}
	};

	class CORE_API Variant
	{
	public:
		Variant() : ValueType(0)
		{}

		template<typename PtrType>
		Variant(PtrType* ptr) : ValueType(GetTypeId<PtrType*>())
		{
			
		}

		inline bool HasValue() const
		{
			return ValueType > 0;
		}

		inline TypeId GetValueType() const
		{
			return ValueType;
		}

		template<typename T>
		inline T GetValue() const
		{
			if (ValueType != GetTypeId<T>())
			{
				throw VariantCastException("Failed to cast Variant to the return type!");
			}
			return *(T*)this;
		}

		inline void* GetRawPtrValue() const
		{
			return PtrValue;
		}

		template<typename PtrType>
		inline PtrType* GetPointerValue() const
		{
			return (PtrType*)PtrValue;
		}

	private:
		union
		{
			bool   BoolValue;
			String StringValue;
			s8	   Int8Value;
			s16    Int16Value;
			s32    Int32Value;
			s64    Int64Value;
			u8     Uint8Value;
			u16    Uint16Value;
			u32    Uint32Value;
			u64    Uint64Value;
			f32    Float32Value;
			f64    Float64Value;
			void*  PtrValue;
		};

		TypeId ValueType = 0;
	};

#pragma pack(push, 1)


	namespace Internal
	{

		//template<typename T>
		//struct TypeInfoImpl
		//{
		//	const TypeInfo TypeInfo;
		//	const TypeData<T> TypeData;
		//};
	}


#pragma pack(pop)

	// **********************************************************
	// Struct Type
	

	//template<class ClassType, class FieldType>
	//void AddProperty(const char* name, FieldType ClassType::* member, const char* attributes)
	//{
	//	//using UnderlyingType = AZStd::RemoveEnumT<FieldType>;
	//	using ValueType = std::remove_pointer_t<FieldType>;
	//	std::cout << "Type: " << typeid(ValueType).name() << " id: " << GetTypeId<ValueType>() << "\n";
	//}


} // namespace CE
