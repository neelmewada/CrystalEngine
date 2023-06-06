#pragma once

#include "Misc/CoreDefines.h"
#include "Misc/CoreMacros.h"
#include "Containers/String.h"
#include "Containers/Array.h"
#include "Containers/HashMap.h"
#include "CoreTypes.h"

#include "RTTIDefines.h"

#include "Math/Math.h"
#include "Math/Vector.h"

namespace CE
{

	// Forward Declarations
	

	// **********************************************************
	// Variant

	class CORE_API VariantCastException : public std::runtime_error
	{
	public:
		VariantCastException(String message) : std::runtime_error(message.ToStdString())
		{}
	};

	class CORE_API Variant
	{
	public:
		Variant() : valueTypeId(0)
		{
			memset(this, 0, sizeof(Variant));
		}

		Variant(const Variant& copy)
		{
			Free();
			CopyFrom(copy);
		}

		CE::Variant& operator=(const CE::Variant& copy)
		{
			Free();
			CopyFrom(copy);
			return *this;
		}

		~Variant();

		// Internal use only!
		void CopyFrom(const Variant& copy);

		// Internal use only! Frees up memory used by subvalues
		void Free();

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
        
        Variant(Vec2  value) { SetInternalValue(value); }
        Variant(Vec3  value) { SetInternalValue(value); }
        Variant(Vec4  value) { SetInternalValue(value); }

        Variant(String value) { Clear(); StringValue = value; valueTypeId = TYPEID(String); }

		Variant(IO::Path value) 
		{
			SetInternalValue(value);
		}
		
		Variant(Name value) 
		{
			SetInternalValue(value); 
		}

		template<typename ElementType>
		Variant(const CE::Array<ElementType>& value)
		{
			memset(this, 0, sizeof(Variant));
			rawArrayValue = Array<u8>(value.GetSize() * sizeof(ElementType));

			for (int i = 0; i < value.GetSize(); i++)
			{
				ElementType* dest = (ElementType*)(&*rawArrayValue.Begin() + i * sizeof(ElementType));
				*dest = value[i];
			}

			rawArrayValue.ElementTypeId = TYPEID(ElementType);
			valueTypeId = TYPEID(CE::Array<ElementType>);
			arrayElementTypeId = TYPEID(ElementType);
			isArray = true;
		}

		template<>
		Variant(const CE::Array<String>& value)
		{
			memset(this, 0, sizeof(Variant));

			stringArrayValue = value;
			valueTypeId = TYPEID(CE::Array<String>);
			arrayElementTypeId = TYPEID(String);
			isArray = true;
		}

		template<>
		Variant(const CE::Array<Name>& value)
		{
			memset(this, 0, sizeof(Variant));

			nameArrayValue = value;
			valueTypeId = TYPEID(CE::Array<Name>);
			arrayElementTypeId = TYPEID(Name);
			isArray = true;
		}

		template<>
		Variant(const CE::Array<IO::Path>& value)
		{
			memset(this, 0, sizeof(Variant));

			pathArrayValue = value;
			valueTypeId = TYPEID(CE::Array<IO::Path>);
			arrayElementTypeId = TYPEID(IO::Path);
			isArray = true;
		}

		template<typename PtrType>
		Variant(PtrType* ptr) : valueTypeId(TYPEID(PtrType)), PtrValue(ptr)
		{
			isPointer = true;
		}

		template<typename PtrType>
		Variant(const PtrType* ptr) : valueTypeId(TYPEID(PtrType)), PtrValue((void*)ptr)
		{
			isPointer = true;
		}

		template<>
		Variant(char* value)
		{
			Clear();
			StringValue = value;
			valueTypeId = TYPEID(String);
		}

		template<>
		Variant(const char* value)
		{
			Clear();
			StringValue = value; 
			valueTypeId = TYPEID(String);
		}

		template<typename T> requires std::is_enum<T>::Value
		Variant(T enumValue)
		{
			SetInternalValue((std::underlying_type_t<T>)enumValue);
			valueTypeId = TYPEID(T);
		}

		CE_INLINE bool HasValue() const
		{
			return valueTypeId > 0;
		}

		template<typename T>
		CE_INLINE bool IsOfType() const
		{
			return valueTypeId == TYPEID(T);
		}

		CE_INLINE TypeId GetValueTypeId() const
		{
			return valueTypeId;
		}

		CE_INLINE bool IsPointer() const
		{
			return isPointer;
		}

		CE_INLINE bool IsArray() const
		{
			return isArray;
		}

		template<typename T>
		CE_INLINE bool IsArrayElementOfType() const
		{
			return arrayElementTypeId == TYPEID(T);
		}

		template<typename T>
		CE_INLINE T GetValue() const
		{
			if (valueTypeId != GetTypeId<T>())
			{
				throw VariantCastException(String::Format("Failed to cast Variant to the return type"));
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

		template<>
		CE_INLINE Array<String> GetValue() const
		{
			if (valueTypeId != TYPEID(Array<String>))
			{
				throw VariantCastException("Failed to cast Variant to the return type of Array<String> because it is not an array");
			}
			if (arrayElementTypeId != TYPEID(String))
			{
				throw VariantCastException("Failed to cast Variant to the return type of Array<String> because of Element Type");
			}

			return stringArrayValue;
		}

		template<>
		CE_INLINE Array<Name> GetValue() const
		{
			if (valueTypeId != TYPEID(Array<Name>))
			{
				throw VariantCastException("Failed to cast Variant to the return type of Array<Name> because it is not an array");
			}
			if (arrayElementTypeId != TYPEID(Name))
			{
				throw VariantCastException("Failed to cast Variant to the return type of Array<Name> because of Element Type");
			}

			return nameArrayValue;
		}

		template<>
		CE_INLINE Array<IO::Path> GetValue() const
		{
			if (valueTypeId != TYPEID(Array<IO::Path>))
			{
				throw VariantCastException("Failed to cast Variant to the return type of Array<IO::Path> because it is not an array");
			}
			if (arrayElementTypeId != TYPEID(IO::Path))
			{
				throw VariantCastException("Failed to cast Variant to the return type of Array<IO::Path> because of Element Type");
			}

			return pathArrayValue;
		}

		CE_INLINE void* GetRawPtrValue() const
		{
			return PtrValue;
		}

		template<typename PtrType>
		CE_INLINE PtrType* TryGetPointerValue() const
		{
			if (TYPEID(PtrType) != valueTypeId)
				return nullptr;
			return (PtrType*)PtrValue;
		}

		template<typename PtrType>
		CE_INLINE PtrType* GetPointerValue() const
		{
			return (PtrType*)PtrValue;
		}

	private:

		template<typename T>
		CE_INLINE void SetInternalValue(const T& value)
		{
			memset(this, 0, sizeof(Variant)); // Initialze memory to 0
			valueTypeId = TYPEID(T);
			*(T*)this = value;
		}

		/// Initializes/sets the memory block to zero
		void Clear()
		{
			memset(this, 0, sizeof(Variant)); // Initialze memory to 0
		}

		union
		{
			// Simple types
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
            
            Vec2   Vec2Value;
            Vec3   Vec3Value;
            Vec4   Vec4Value;

			IO::Path pathValue;
			CE::Name nameValue;

			// Arrays
			CE::Array<u8> rawArrayValue;
			CE::Array<String> stringArrayValue;
			CE::Array<Name> nameArrayValue;
			CE::Array<IO::Path> pathArrayValue;
		};

		TypeId valueTypeId = 0;
		TypeId arrayElementTypeId = 0;
		bool isPointer = false;
		bool isArray = false;
	};

} // namespace CE
