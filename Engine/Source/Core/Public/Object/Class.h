#pragma once

#include "RTTI.h"

namespace CE
{

	class StructType : public TypeInfo
	{
	protected:
		StructType(const char* name) : TypeInfo(name)
		{}

		template<typename T>
		friend const TypeInfo* GetStaticType<T>();

	public:

		virtual bool IsStruct() override { return true; }

	};

	class ClassType : public StructType
	{
	protected:
		ClassType(const char* name) : StructType(name)
		{}

		template<typename T>
		friend const TypeInfo* GetStaticType<T>();

	public:

		// TypeData is always located AFTER TypeInfo in memory
		virtual const u8* GetRawTypeData() const override { return (u8*)(this + 1); }

		virtual bool IsStruct() override { return false; }
		virtual bool IsClass() override { return true; }
	};

}

#define CE_RTTI_DECLARE_CLASS(Class, ...)\
namespace CE\
{\
	template<>\
	struct TypeData<Class> : public Internal::TypeDataImpl<Class, __VA_ARGS__>\
	{\
	};\
	namespace Internal\
	{\
		template<>\
		struct TypeInfoImpl<Class>\
		{\
			const ClassType Type;\
			const TypeData<Class> TypeData;\
		};\
	}\
	template<>\
	inline const TypeInfo* GetStaticType<Class>()\
	{\
		static Internal::TypeInfoImpl<Class> instance{ ClassType{#Class}, TypeData<Class>() };\
		return &instance.Type;\
	}\
}

#define CE_RTTI_IMPLEMENT_CLASS(Class)\
const TypeInfo* Class::Type()\
{\
	return CE::template GetStaticType<Self>();\
}


/*
template<>\
const TypeInfo* GetStaticType<Class>()\
{\
	static Internal::TypeInfoImpl<Class> instance{ ClassType{#Class}, TypeData<Class>() };\
	return &instance.Type;\
}\
*/

