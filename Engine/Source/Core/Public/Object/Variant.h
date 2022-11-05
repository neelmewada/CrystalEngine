#pragma once

#include "Misc/CoreDefines.h"
#include "Misc/CoreMacros.h"
#include "Containers/String.h"
#include "Containers/Array.h"
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
		Variant() : ValueTypeId(0)
		{}

		Variant(f32 value) { SetInternalValue(value); }
		Variant(f64 value) { SetInternalValue(value); }

		Variant(u8  value) { SetInternalValue(value); }
		Variant(u16 value) { SetInternalValue(value); }
		Variant(u32 value) { SetInternalValue(value); }
		Variant(u64 value) { SetInternalValue(value); }

		Variant(s8  value) { SetInternalValue(value); }
		Variant(s16 value) { SetInternalValue(value); }
		Variant(s32 value) { SetInternalValue(value); }
		Variant(s64 value) { SetInternalValue(value); }

		Variant(bool value) { SetInternalValue(value); }

		Variant(const char* value) { SetInternalValue(String(value)); }
		Variant(String value) { SetInternalValue(value); }

		template<typename ElementType>
		Variant(CE::Array<ElementType> value) : ValueTypeId(GetTypeId<CE::Array<ElementType>>())
		{

		}

		template<typename PtrType>
		Variant(PtrType* ptr) : ValueTypeId(GetTypeId<PtrType*>()), PtrValue(ptr)
		{}

		inline bool HasValue() const
		{
			return ValueTypeId > 0;
		}

		inline TypeId GetValueTypeId() const
		{
			return ValueTypeId;
		}

		template<typename T>
		inline T GetValue() const
		{
			if (ValueTypeId != GetTypeId<T>())
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

		template<typename T>
		void SetInternalValue(T value)
		{
			ValueTypeId = GetTypeId<T>();
			*(T*)this = value;
		}

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

			// Complex types
			CE::Array<Variant> ArrayValue;
		};

		TypeId ValueTypeId = 0;
	};

} // namespace CE
