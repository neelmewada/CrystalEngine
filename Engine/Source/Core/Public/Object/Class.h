#pragma once

#include "RTTI.h"
#include "TypeTraits.h"
#include "Variant.h"

#include "Field.h"
#include "Function.h"

#include "Containers/Array.h"

#include <iostream>
#include <utility>

namespace CE
{
	// Forward Decls

	class StructType;
	class ClassType;
	class Variant;

	template<typename ElementType>
	class Array;

	namespace Internal
	{
		template<typename Struct>
		struct TypeInfoImpl;
	}

	// *************************************************
	// RTTI

	// Default implementation always returns nullptr. Specialization will return the correct data
	template<typename Type>
	const StructType* GetStaticStruct()
	{
		return nullptr;
	}

	// Default implementation always returns nullptr. Specialization will return the correct data
	template<typename Type>
	const ClassType* GetStaticClass()
	{
		return nullptr;
	}


	// *************************************************
	// Struct Type

	class CORE_API StructType : public TypeInfo
	{
	protected:
		StructType(String name, String attributes = "") : TypeInfo(name, attributes)
		{}

		template<typename T>
		friend const TypeInfo* GetStaticType();

		template<typename Struct>
		friend struct CE::Internal::TypeInfoImpl;

	public:

		// TypeData is always located AFTER TypeInfo in memory
		virtual const u8* GetRawTypeData() const { return (u8*)(this + 1); }

		virtual TypeId GetTypeId() const override
		{
			return *(TypeId*)(GetRawTypeData() + sizeof(TypeId));
		}

		virtual bool IsStruct() const override { return true; }

		virtual bool IsAssignableTo(TypeId typeId) const override;

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

		inline u32 GetLocalFieldCount() const
		{
			return LocalFields.GetSize();
		}

		inline const FieldType* GetLocalFieldAt(u32 index) const
		{
			return &LocalFields[index];
		}

		inline u32 GetLocalFunctionCount() const
		{
			return LocalFunctions.GetSize();
		}

		inline const FunctionType* GetLocalFunctionAt(u32 index) const
		{
			return &LocalFunctions[index];
		}

		inline u32 GetSuperTypesCount() const
		{
			return SuperTypeIds.GetSize();
		}

		inline TypeId GetSuperType(s32 index) const
		{
			return SuperTypeIds[index];
		}

		FieldType* GetFirstField();

	protected:

		virtual void CacheAllFields();

		template<typename Struct, typename Field>
		inline void AddField(const char* name, Field Struct::* field, SIZE_T offset, const char* attributes)
		{
			LocalFields.Add(FieldType(name, CE::GetTypeId<Field>(), sizeof(Field), offset, attributes, this));
		}

	private:

		template<typename ReturnType, typename ClassOrStruct, typename... Args, std::size_t... Is>
		void AddFunction(const char* name, ReturnType(ClassOrStruct::* function)(Args...), const char* attributes, std::index_sequence<Is...>) 
		{
			ReturnType(ClassOrStruct::*funcPtr)(Args...) = function;

			FunctionDelegate funcDelegate = [funcPtr](Object* instance, std::initializer_list<CE::Variant> params, CE::Variant& returnValue) -> void
			{
				if constexpr (std::is_same_v<ReturnType, void>) // No return value
				{
					(((ClassOrStruct*)instance)->*funcPtr)( ((params.begin() + Is)->GetValue<Args>())...);
					returnValue = CE::Variant();
				}
				else
				{
					auto value = (((ClassOrStruct*)instance)->*funcPtr)( ((params.begin() + Is)->GetValue<Args>())... );
					returnValue = CE::Variant(value);
				}
			};

			LocalFunctions.Add(FunctionType(name, CE::GetTypeId<ReturnType>(), { CE::GetTypeId<Args>()... }, funcDelegate, attributes));
		}

		template<typename ReturnType, typename ClassOrStruct, typename... Args, std::size_t... Is>
		void AddFunction(const char* name, ReturnType(ClassOrStruct::* function)(Args...) const, const char* attributes, std::index_sequence<Is...>) 
		{
			ReturnType(ClassOrStruct::*funcPtr)(Args...) const = function;

			FunctionDelegate funcDelegate = [funcPtr](Object* instance, std::initializer_list<CE::Variant> params, CE::Variant& returnValue) -> void
			{
				if constexpr (std::is_same_v<ReturnType, void>) // No return value
				{
					(((ClassOrStruct*)instance)->*funcPtr)(((params.begin() + Is)->GetValue<Args>())...);
					returnValue = CE::Variant();
				}
				else
				{
					auto value = (((ClassOrStruct*)instance)->*funcPtr)(((params.begin() + Is)->GetValue<Args>())...);
					returnValue = CE::Variant(value);
				}
			};

			LocalFunctions.Add(FunctionType(name, CE::GetTypeId<ReturnType>(), { CE::GetTypeId<Args>()... }, funcDelegate, String(attributes) + ",Constant"));
		}

	protected:

		template<typename ReturnType, typename ClassOrStruct, typename... Args>
		void AddFunction(const char* name, ReturnType (ClassOrStruct::* function)(Args...), const char* attributes)
		{
			AddFunction<ReturnType, ClassOrStruct, Args...>(name, function, attributes, std::make_index_sequence<sizeof...(Args)>());
		}

		template<typename ReturnType, typename ClassOrStruct, typename... Args>
		void AddFunction(const char* name, ReturnType(ClassOrStruct::* function)(Args...) const, const char* attributes)
		{
			AddFunction<ReturnType, ClassOrStruct, Args...>(name, function, attributes, std::make_index_sequence<sizeof...(Args)>());
		}

		template<typename... SuperTypes>
		inline void AddSuper()
		{
			TypeId ids[] = { CE::GetTypeId<SuperTypes>()... };
			constexpr int count = sizeof(ids) / sizeof(TypeId);
			for (int i = 0; i < count; i++)
			{
				SuperTypeIds.Add(ids[i]);
			}
		}

		template<>
		inline void AddSuper<>()
		{
			
		}

		// Inherited + Local fields
		CE::Array<FieldType> CachedFields;

		CE::Array<FieldType> LocalFields;
		CE::Array<TypeId> SuperTypeIds;
		CE::Array<FunctionType> LocalFunctions;
		bool bFieldsCached = false;
	};


	// *************************************************
	// Class Type

	namespace Internal
	{
		class CORE_API IClassTypeImpl
		{
		public:
			virtual void* CreateDefaultInstance() = 0;
			virtual void DestroyInstance(void* instance) = 0;
		};
	}

	class CORE_API ClassType : public StructType
	{
	protected:
		ClassType(String name, Internal::IClassTypeImpl* impl, String attributes = "") : StructType(name, attributes), Impl(impl)
		{}

		template<typename T>
		friend const TypeInfo* GetStaticType();

		template<typename Class>
		friend struct CE::Internal::TypeInfoImpl;

	public:

		// TypeData is always located AFTER TypeInfo in memory
		virtual const u8* GetRawTypeData() const override { return (u8*)(this + 1); }

		virtual TypeId GetTypeId() const override
		{
			return *(TypeId*)(GetRawTypeData() + sizeof(TypeId));
		}

		virtual bool IsStruct() const override { return false; }
		virtual bool IsClass() const override { return true; }

		

	private:
		Internal::IClassTypeImpl* Impl = nullptr;

	};

#pragma pack(push, 1)

	namespace Internal
	{
		// Specialization will contain the required data
		template<typename T>
		struct TypeInfoImpl
		{
			//const TypeInfo TypeInfo;
			//const StructTypeData<T> TypeData;
		};

		// Recursively populates the StructTypeData
		// Layout of typeData:
		// [ TypeIdSize size, TypeId firstTypeId ... TypeId lastTypeId, PtrDiff offset/endMarker if = 0,
		// TypeIdSize size, TypeId firstTypeId ... TypeId lastTypeId, PtrDiff offset/endMarker if = 0... ]
		// Each block represents inherited types from a base, the first block doesn't need offset as it is implicitly 0
		// Therefore we can use the offset as an end marker, all other bases will have a positive offset
		template<typename... BaseTypes>
		struct BaseStructTypeData
		{};

		// Specialization of struct BaseStructTypeData<BaseTypes...>;
		template<typename Base1, typename Base2, typename... BaseN>
		struct BaseStructTypeData<Base1, Base2, BaseN...>
		{
			template<typename Derived>
			void FillBaseStructTypeData(PtrDiff inOffset, TypeIdSize& outHeadSize)
			{
				BaseStructTypeData1.template FillBaseStructTypeData<Derived>(ComputePointerOffset<Derived, Base1>(), outHeadSize);

				Offset = ComputePointerOffset<Derived, Base2>();
				BaseStructTypeDataRemaining.template FillBaseStructTypeData<Derived>(Offset, Size);
			}

			BaseStructTypeData<Base1> BaseStructTypeData1;
			PtrDiff Offset;
			TypeIdSize Size;
			BaseStructTypeData<Base2, BaseN...> BaseStructTypeDataRemaining;
		};

		template<typename Base>
		struct BaseStructTypeData<Base>
		{
			template<typename Derived>
			void FillBaseStructTypeData(PtrDiff inOffset, TypeIdSize& outHeadSize)
			{
				StructTypeData<Base>* baseTypeData = nullptr;
				if (GetStaticStruct<Base>() != nullptr)
				{
					baseTypeData = (StructTypeData<Base>*)(GetStaticStruct<Base>()->GetRawTypeData());
				}
				else if (GetStaticClass<Base>() != nullptr)
				{
					baseTypeData = (StructTypeData<Base>*)(GetStaticClass<Base>()->GetRawTypeData());
				}
				else
				{
					throw std::runtime_error(std::string("Failed to find struct or class ") + typeid(Base).name());
					return;
				}

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
			char Data[sizeof(StructTypeData<Base>) - sizeof(PtrDiff) - sizeof(TypeIdSize)];
		};

		// Main template for TypeDataImpl
		template<typename Type, typename... BaseTypes>
		struct TypeDataImpl
		{
			TypeDataImpl()
			{
				this->ThisTypeId = CE::GetTypeId<Type>();
				BaseStructTypeData.template FillBaseStructTypeData<Type>(0, Size);
				Size++;
				EndMarker = 0;
			}

			const char* GetData() const { return (char*)&ThisTypeId; }

			TypeId GetTypeId() const { return ThisTypeId; }

			TypeIdSize Size;
			TypeId ThisTypeId;
			BaseStructTypeData<BaseTypes...> BaseStructTypeData;
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

	

}

#define __CE_SUPER_LIST(...) CE_EXPAND(CE_CONCATENATE(__CE_SUPER_LIST_,CE_ARG_COUNT(__VA_ARGS__)))(__VA_ARGS__)

#define CE_FIELD_LIST(x) x
#define CE_FIELD(FieldName, ...) Type.AddField(#FieldName, &Self::FieldName, offsetof(Self, FieldName), "" #__VA_ARGS__);

#define CE_FUNCTION_LIST(x) x
#define CE_FUNCTION(FunctionName, ...) Type.AddFunction(#FunctionName, &Self::FunctionName, "" #__VA_ARGS__);

#define __CE_RTTI_JOIN_CLASSES_0()
#define __CE_RTTI_JOIN_CLASSES_1(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_2(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_3(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_4(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_5(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_6(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_7(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_8(...) , __VA_ARGS__

#define __CE_RTTI_JOIN_CLASSES(Class, ...) Class CE_EXPAND(CE_CONCATENATE(__CE_RTTI_JOIN_CLASSES_,CE_ARG_COUNT(__VA_ARGS__)))(__VA_ARGS__)

#define CE_SUPER(...) __VA_ARGS__
#define CE_ATTRIBS(...) __VA_ARGS__

#define __CE_NEW_INSTANCE_(Namespace, Class) new Namespace::Class
#define __CE_NEW_INSTANCE_NotAbstract(Namespace, Class) new Namespace::Class
#define __CE_NEW_INSTANCE_false(Namespace, Class) new Namespace::Class

#define __CE_NEW_INSTANCE_Abstract(Namespace, Class) nullptr
#define __CE_NEW_INSTANCE_true(Namespace, Class) nullptr

#define CE_ABSTRACT Abstract
#define CE_NOT_ABSTRACT NotAbstract
#define CE_DONT_INSTANTIATE Abstract
#define CE_INSTANTIATE NotAbstract

#define CE_RTTI_CLASS(API, Namespace, Class, SuperClasses, IsAbstract, Attributes, FieldList, FunctionList)\
namespace CE\
{\
	template<>\
	struct StructTypeData<Namespace::Class> : public Internal::TypeDataImpl<__CE_RTTI_JOIN_CLASSES(Namespace::Class, SuperClasses)>\
	{\
	};\
	namespace Internal\
	{\
		template<>\
		struct TypeInfoImpl<Namespace::Class> : public CE::Internal::IClassTypeImpl\
		{\
            typedef Namespace::Class Self;\
            CE::ClassType Type;\
			const CE::StructTypeData<Namespace::Class> TypeData;\
            TypeInfoImpl(CE::ClassType type, CE::StructTypeData<Namespace::Class> typeData) : Type(type), TypeData(typeData)\
            {\
                TypeInfo::RegisterType(&Type);\
                FieldList\
				FunctionList\
				Type.AddSuper<SuperClasses>();\
            }\
			virtual void* CreateDefaultInstance() override\
			{\
				return CE_EXPAND(CE_CONCATENATE(__CE_NEW_INSTANCE_,CE_FIRST_ARG(IsAbstract)))(Namespace, Class);\
			}\
			virtual void DestroyInstance(void* instance) override\
			{\
				if (instance != nullptr) delete (Namespace::Class*)instance;\
			}\
		};\
	}\
	template<>\
	inline const TypeInfo* GetStaticType<Namespace::Class>()\
	{\
        static Internal::TypeInfoImpl<Namespace::Class> instance{ ClassType{ #Namespace "::" #Class, &instance, #Attributes "" }, StructTypeData<Namespace::Class>() };\
		return &instance.Type;\
	}\
	template<>\
	inline const ClassType* GetStaticClass<Namespace::Class>()\
	{\
		return (ClassType*)GetStaticType<Namespace::Class>();\
	}\
}

#define CE_RTTI_CLASS_IMPL(API, Namespace, Class)\
const CE::ClassType* Namespace::Class::Type()\
{\
	return (CE::ClassType*)(CE::template GetStaticType<Self>());\
}

#define __CE_RTTI_SUPERCLASS_0(...) typedef void Super;
#define __CE_RTTI_SUPERCLASS_1(SuperClass) typedef SuperClass Super;
#define __CE_RTTI_SUPERCLASS_2(SuperClass, ...) __CE_RTTI_SUPERCLASS_1(SuperClass);
#define __CE_RTTI_SUPERCLASS_3(SuperClass, ...) __CE_RTTI_SUPERCLASS_1(SuperClass);
#define __CE_RTTI_SUPERCLASS_4(SuperClass, ...) __CE_RTTI_SUPERCLASS_1(SuperClass);
#define __CE_RTTI_SUPERCLASS_5(SuperClass, ...) __CE_RTTI_SUPERCLASS_1(SuperClass);
#define __CE_RTTI_SUPERCLASS_6(SuperClass, ...) __CE_RTTI_SUPERCLASS_1(SuperClass);
#define __CE_RTTI_SUPERCLASS_7(SuperClass, ...) __CE_RTTI_SUPERCLASS_1(SuperClass);
#define __CE_RTTI_SUPERCLASS_8(SuperClass, ...) __CE_RTTI_SUPERCLASS_1(SuperClass);

#define __CE_RTTI_SUPERCLASS(...) CE_MACRO_EXPAND(CE_CONCATENATE(__CE_RTTI_SUPERCLASS_, CE_ARG_COUNT(__VA_ARGS__)))(__VA_ARGS__)

#define CE_CLASS(Class, ...)\
public:\
	template<typename T>\
	friend struct CE::Internal::TypeInfoImpl;\
    typedef Class Self;\
    __CE_RTTI_SUPERCLASS(__VA_ARGS__)\
    static const ClassType* Type();\
    virtual const TypeInfo* GetType() const\
    {\
        return Type();\
    }



#define CE_RTTI_STRUCT(API, Namespace, Struct, SuperStructs, Attributes, FieldList)\
namespace CE\
{\
	template<>\
	struct StructTypeData<Namespace::Struct> : public Internal::TypeDataImpl<__CE_RTTI_JOIN_CLASSES(Namespace::Struct, SuperStructs)>\
	{\
	};\
	namespace Internal\
	{\
		template<>\
		struct TypeInfoImpl<Namespace::Struct>\
		{\
            typedef Namespace::Struct Self;\
            CE::StructType Type;\
			const CE::StructTypeData<Namespace::Struct> TypeData;\
            TypeInfoImpl(CE::StructType type, CE::StructTypeData<Namespace::Struct> typeData) : Type(type), TypeData(typeData)\
            {\
                TypeInfo::RegisterType(&Type);\
                FieldList\
				Type.AddSuper<SuperStructs>();\
            }\
		};\
	}\
	template<>\
	inline const TypeInfo* GetStaticType<Namespace::Struct>()\
	{\
        static Internal::TypeInfoImpl<Namespace::Struct> instance{ StructType{ #Namespace "::" #Struct, #Attributes "" }, StructTypeData<Namespace::Struct>() };\
		return &instance.Type;\
	}\
	template<>\
	inline const StructType* GetStaticStruct<Namespace::Struct>()\
	{\
		return (StructType*)GetStaticType<Namespace::Struct>();\
	}\
}

#define CE_RTTI_STRUCT_IMPL(API, Namespace, Struct)\
const CE::StructType* Namespace::Struct::Type()\
{\
	return (CE::StructType*)(CE::template GetStaticType<Self>());\
}

#define CE_STRUCT(Struct, ...)\
public:\
	template<typename T>\
	friend struct CE::Internal::TypeInfoImpl;\
    typedef Struct Self;\
    __CE_RTTI_SUPERCLASS(__VA_ARGS__)\
    static const StructType* Type();\
    virtual const TypeInfo* GetType() const\
    {\
        return Type();\
    }

