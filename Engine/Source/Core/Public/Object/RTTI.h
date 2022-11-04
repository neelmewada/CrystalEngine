#pragma once

#include "Misc/CoreDefines.h"
#include "Misc/CoreMacros.h"

#include <type_traits>


namespace CE
{

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

		typedef std::remove_pointer<Type>::type TypeWithoutPtr;
		typedef std::remove_cv<TypeWithoutPtr>::type FinalType;

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
		const char* Name;

		TypeInfo(const char* name) : Name(name)
		{}

	public:
		const char* GetName() const { return Name; }

		// TypeData is always located AFTER TypeInfo in memory
		virtual const u8* GetRawTypeData() const { return (u8*)(this + 1); }

		virtual bool IsClass() { return false; }
		virtual bool IsStruct() { return false; }
		virtual bool IsField() { return false; }

		TypeId GetTypeId() const
		{
			return *(TypeId*)(GetRawTypeData() + sizeof(TypeId));
		}

		IntPtr TryCast(IntPtr ptr, TypeId castToType) const
		{
			const u8* data = GetRawTypeData();
			SIZE_T byteIndex = 0;
			PtrDiff offset = 0;

			while (true)
			{
				TypeIdSize size = *(TypeIdSize*)(data + byteIndex);
				byteIndex += sizeof(TypeIdSize);

				for (TypeIdSize i = 0; i < size; i++, byteIndex += sizeof(TypeIdSize))
				{
					if (*(TypeIdSize*)(data + byteIndex) == castToType)
						return ptr + offset;
				}

				offset = *(PtrDiff*)(data + byteIndex);

				if (offset == 0)
					return 0;

				byteIndex += sizeof(PtrDiff);
			}

			return 0;
		}
	};

#pragma pack(push, 1)

	// Default implementation always returns nullptr. Specialization will return the correct data
	template<typename Type>
	const TypeInfo* GetStaticType();

	// Specialization will contain the magic data.
	template<typename T>
	struct TypeData
	{};

	namespace Internal
	{
		// Specialization will contain the required data
		template<typename T>
		struct TypeInfoImpl
		{
			//const TypeInfo TypeInfo;
			//const TypeData<T> TypeData;
		};

		// Recursively populates the TypeData
		// Layout of typeData:
		// [ TypeIdSize size, TypeId firstTypeId ... TypeId lastTypeId, PtrDiff offset/endMarker if = 0,
		// TypeIdSize size, TypeId firstTypeId ... TypeId lastTypeId, PtrDiff offset/endMarker if = 0... ]
		// Each block represents inherited types from a base, the first block doesn't need offset as it is implicitly 0
		// Therefore we can use the offset as an end marker, all other bases will have a positive offset
		template<typename... BaseTypes>
		struct BaseTypeData
		{};

		// Specialization of struct BaseTypeData<BaseTypes...>;
		template<typename Base1, typename Base2, typename... BaseN>
		struct BaseTypeData<Base1, Base2, BaseN...>
		{
			template<typename Derived>
			void FillBaseTypeData(PtrDiff inOffset, TypeIdSize& outHeadSize)
			{
				BaseTypeData1.template FillBaseTypeData<Derived>(ComputePointerOffset<Derived, Base1>(), outHeadSize);

				Offset = ComputePointerOffset<Derived, Base2>();
				BaseTypeDataRemaining.template FillBaseTypeData<Derived>(Offset, Size);
			}

			BaseTypeData<Base1> BaseTypeData1;
			PtrDiff Offset;
			TypeIdSize Size;
			BaseTypeData<Base2, BaseN...> BaseTypeDataRemaining;
		};

		template<typename Base>
		struct BaseTypeData<Base>
		{
			template<typename Derived>
			void FillBaseTypeData(PtrDiff inOffset, TypeIdSize& outHeadSize)
			{
				const TypeData<Base>* baseTypeData = (TypeData<Base>*)(GetStaticType<Base>()->GetRawTypeData());

				// return size of head list
				outHeadSize = baseTypeData->Size;

				const char* data = baseTypeData->GetData();
				SIZE_T byteSize = baseTypeData->Size * sizeof(TypeIdSize);

				// copy type list
				memcpy(Data, data, byteSize);

				SIZE_T byteIndex = byteSize;
				PtrDiff offset = *(PtrDiff*)(data + byteIndex);

				while (offset != 0)
				{
					// fill next offset and add pointer offset
					*(PtrDiff*)(Data + byteIndex) = offset + inOffset;
					byteIndex += sizeof(PtrDiff);

					// fill next size
					const TypeIdSize size = *(TypeIdSize*)(data + byteIndex);
					*(TypeIdSize*)(Data + byteIndex) = size;

					byteSize = size * sizeof(TypeIdSize);
					byteIndex += sizeof(TypeIdSize);

					// copy types
					memcpy(Data + byteIndex, data + byteIndex, byteSize);
					byteIndex += byteSize;
					offset = *(PtrDiff*)(data + byteIndex);
				}
			}

			// We only need the previous type data array, but not its size or end marker as we will insert them ourselves
			char Data[sizeof(TypeData<Base>) - sizeof(PtrDiff) - sizeof(TypeIdSize)];
		};

		// Main template for TypeDataImpl
		template<typename Type, typename... BaseTypes>
		struct TypeDataImpl
		{
			TypeDataImpl()
			{
				this->ThisTypeId = GetTypeId<Type>();
				BaseTypeData.template FillBaseTypeData<Type>(0, Size);
				Size++;
				EndMarker = 0;
			}

			const char* GetData() const { return (char*)&ThisTypeId; }

			TypeIdSize Size;
			TypeId ThisTypeId;
			BaseTypeData<BaseTypes...> BaseTypeData;
			PtrDiff EndMarker;
		};

		// Specialization of TypeDataImpl<Type, BaseTypes...> for a type with no base types
		template<typename Type>
		struct TypeDataImpl<Type>
		{
			TypeDataImpl() : Size(1), EndMarker(0)
			{
				this->ThisTypeId = GetTypeId<Type>();
			}

			const char* GetData() const { return (char*)&ThisTypeId; }

			TypeIdSize Size;
			TypeId ThisTypeId;
			PtrDiff EndMarker;
		};
	} // namespace Internal

#pragma pack(pop)

} // namespace CE

// RTTI Macros

#define __CE_RTTI_SUPERCLASS_0(...) typedef void Super;
#define __CE_RTTI_SUPERCLASS_1(SuperClass) typedef SuperClass Super;

#define __CE_RTTI_SUPERCLASS(SuperClass) CE_MACRO_EXPAND(CE_CONCATENATE(__CE_RTTI_SUPERCLASS_, CE_ARG_COUNT(SuperClass)))(SuperClass)

#define CE_RTTI(Class, SuperClass)\
public:\
	typedef Class Self;\
	__CE_RTTI_SUPERCLASS(SuperClass)\
	static const TypeInfo* Type();\
	virtual const TypeInfo* GetType() const\
	{\
		return Type();\
	}

