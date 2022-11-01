#pragma once

#include "Misc/CoreDefines.h"
#include "Containers/String.h"
#include "Containers/HashMap.h"

#include <type_traits>
#include <any>
#include <iostream>

namespace CE
{

	// Forward Declarations
	

	// **********************************************************
	// Variant

	class CORE_API Variant
	{
	public:
		Variant() : bHasValue(false)
		{}

		inline bool HasValue()
		{
			return bHasValue;
		}

	private:
		union
		{
			bool boolValue;
			String stringValue;
			s8 s8Value;
			s16 s16Value;
		};

		bool bHasValue = false;
	};

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
		inline static ptrdiff_t ComputePointerOffset()
		{
			Derived* derivedPtr = (Derived*)1;
			Base* basePtr = static_cast<Base*>(derivedPtr);
			return (intptr_t)basePtr - (intptr_t)derivedPtr;
		}
	}

	template<typename Type>
	TypeId GetTypeId()
	{
		static TypeId typeId = Internal::GenerateNewTypeId();
		return typeId;
	}

	struct TypeInfo
	{
		const char* Name;

		const char* GetName() const { return Name; }
		const char* GetTypeData() const { return (char*)(this + 1); }

		TypeId GetTypeId() const 
		{ 
			return *(TypeId*)(GetTypeData() + sizeof(TypeId));
		}

		intptr_t TryCast(intptr_t ptr, TypeId castToType) const
		{
			const char* data = GetTypeData();
			size_t byteIndex = 0;
			ptrdiff_t offset = 0;

			while (true)
			{
				TypeIdSize size = *(TypeIdSize*)(data + byteIndex);
				byteIndex += sizeof(TypeIdSize);

				for (TypeIdSize i = 0; i < size; i++, byteIndex += sizeof(TypeIdSize))
				{
					if (*(TypeIdSize*)(data + byteIndex) == castToType)
						return ptr + offset;
				}

				offset = *(ptrdiff_t*)(data + byteIndex);

				if (offset == 0)
					return 0;

				byteIndex += sizeof(ptrdiff_t);
			}

			return 0;
		}
	};

	template<typename Type>
	struct TypeDataImpl
	{
		TypeDataImpl()
		{
			this->TypeId = GetTypeId<Type>();
			Size = 1;
			EndMarker = 0;
		}

		const char* GetData() const { return (char*)&TypeId; }

		TypeIdSize Size;
		TypeId TypeId;
		ptrdiff_t EndMarker;
	};

	// Specialization will contain the magic data.
	template<typename T>
	struct TypeData
	{
	};

	// Recursively populates the typeData
	// Layout of typeData:
	// [ typeId_t size, typeId_t firstTypeId ... typeId_t lastTypeId, ptrdiff_t offset/endMarker if = 0,
	// typeId_t size, typeId_t firstTypeId ... typeId_t lastTypeId, ptrdiff_t offset/endMarker if = 0... ]
	// Each block represents inherited types from a base, the first block doesn't need offset as it is implicitly 0
	// Therefore we can use the offset as an end marker, all other bases will have a positive offset
	template<typename... BaseTypes>
	struct BaseTypeData
	{

	};

	template<typename Base>
	struct BaseTypeData<Base>
	{

		void FillBaseTypeData(ptrdiff_t aOffset, TypeId& outHeadSize)
		{
			const TypeData<Base>* baseTypeId = (TypeData<Base>*)(GetTypeInfo<Base>::Get()->GetTypeData());

			// return size of head list
			outHeadSize = baseTypeId->Size;

			const char* data = baseTypeId->GetData();
			size_t byteSize = baseTypeId->Size * sizeof(TypeId);

			// copy type list
			memcpy(Data, data, byteSize);

			size_t byteIndex = byteSize;
			ptrdiff_t offset = *reinterpret_cast<const ptrdiff_t*>(data + byteIndex);
			while (offset != 0)
			{
				// fill next offset and add pointer offset
				*reinterpret_cast<ptrdiff_t*>(Data + byteIndex) = offset + aOffset;
				byteIndex += sizeof(ptrdiff_t);

				// fill next size
				const TypeId size = *reinterpret_cast<const TypeId*>(data + byteIndex);
				*reinterpret_cast<TypeId*>(Data + byteIndex) = size;
				byteSize = size * sizeof(typeId_t);
				byteIndex += sizeof(typeId_t);

				// copy types
				memcpy(Data + byteIndex, data + byteIndex, byteSize);
				byteIndex += byteSize;

				offset = *reinterpret_cast<const ptrdiff_t*>(data + byteIndex);
			}
		}

		// We only need the previous type data array, but not its size or end marker
		char Data[sizeof(TypeData<Base>) - sizeof(ptrdiff_t) - sizeof(TypeId)];
	};

	template<typename Type, typename... BaseTypes>
	struct TypeDataImpl
	{
		TypeDataImpl()
		{
			this->TypeId = GetTypeId<Type>();
			// Fill base types
			Size++; // Size is the base's size + 1 to account for current type id
			EndMarker = 0;
		}

		const char* GetData() const { return (char*)&TypeId; }

		TypeIdSize Size;
		TypeId TypeId;
		BaseTypeData<BaseTypes...> BaseTypeData;
		ptrdiff_t EndMarker;
	};

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
