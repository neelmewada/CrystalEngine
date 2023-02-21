#pragma once

#include "Misc/CoreDefines.h"
#include "Misc/CoreMacros.h"
#include "Containers/String.h"
#include "Containers/Array.h"
#include "Containers/HashMap.h"

#include "RTTIDefines.h"

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
		{
			memset(this, 0, sizeof(Variant));
		}

		Variant(const Variant& copy)
		{
			memcpy(this, &copy, sizeof(Variant));
		}

		CE::Variant& operator=(const CE::Variant& copy)
		{
			memcpy(this, &copy, sizeof(Variant));
			return *this;
		}

		~Variant()
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

		Variant(const char* value) { StringValue = value; ValueTypeId = TYPEID(String); }
		Variant(String value) { StringValue = value; ValueTypeId = TYPEID(String); }

		Variant(IO::Path value) 
		{
			SetInternalValue(value);
		}
		
		Variant(Name value) 
		{
			SetInternalValue(value); 
		}

		template<typename ElementType>
		Variant(CE::Array<ElementType> value)
		{
			memset(this, 0, sizeof(Variant));
			ArrayValue = Array<u8>(value.GetSize() * sizeof(ElementType));

			for (int i = 0; i < value.GetSize(); i++)
			{
				ElementType* dest = (ElementType*)(&*ArrayValue.Begin() + i * sizeof(ElementType));
				*dest = value[i];
			}

			ArrayValue.ElementTypeId = TYPEID(ElementType);
			ValueTypeId = TYPEID(CE::Array<ElementType>);
		}

		template<typename PtrType>
		Variant(PtrType* ptr) : ValueTypeId(GetTypeId<PtrType*>()), PtrValue(ptr)
		{}

		template<typename T> requires std::is_enum<T>::value
		Variant(T enumValue)
		{
			SetInternalValue((std::underlying_type_t<T>)enumValue);
			ValueTypeId = TYPEID(T);
		}

		CE_INLINE bool HasValue() const
		{
			return ValueTypeId > 0;
		}

		template<typename T>
		CE_INLINE bool IsOfType() const
		{
			return ValueTypeId == TYPEID(T);
		}

		CE_INLINE TypeId GetValueTypeId() const
		{
			return ValueTypeId;
		}

		template<typename T>
		CE_INLINE T GetValue() const
		{
			if (ValueTypeId != GetTypeId<T>())
			{
				throw VariantCastException("Failed to cast Variant to the return type!");
			}

			if constexpr (std::is_reference_v<T>)
			{
                typedef typename std::remove_reference<T>::type PlainType;
				return *(PlainType*)this;
			}
			else
			{
				return *(T*)this;
			}
		}

		CE_INLINE void* GetRawPtrValue() const
		{
			return PtrValue;
		}

		template<typename PtrType>
		CE_INLINE PtrType* GetPointerValue() const
		{
			return (PtrType*)PtrValue;
		}

	private:

		template<typename T>
		CE_INLINE void SetInternalValue(T value)
		{
			memset(this, 0, sizeof(Variant));
			ValueTypeId = GetTypeId<T>();
			*(T*)this = value;
		}

		union
		{
			// Simple types
			bool   BoolValue;
			String StringValue{};
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

			CE::Array<u8> ArrayValue;
			IO::Path PathValue;
			CE::Name NameValue;
		};

		TypeId ValueTypeId = 0;
	};

} // namespace CE
