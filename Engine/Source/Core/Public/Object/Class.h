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
		friend const TypeInfo* GetStaticType();

	public:

		virtual bool IsStruct() override { return true; }

	};

	class ClassType : public StructType
	{
	protected:
		ClassType(const char* name) : StructType(name)
		{}

		template<typename T>
		friend const TypeInfo* GetStaticType();

	public:

		// TypeData is always located AFTER TypeInfo in memory
		virtual const u8* GetRawTypeData() const override { return (u8*)(this + 1); }

		virtual bool IsStruct() override { return false; }
		virtual bool IsClass() override { return true; }
	};

}


#define CE_RTTI_DECLARE_CLASS(...)\
namespace CE\
{\
	template<>\
	struct TypeData<CE_FIRST_ARG(__VA_ARGS__)> : public Internal::TypeDataImpl<__VA_ARGS__>\
	{\
	};\
	namespace Internal\
	{\
		template<>\
		struct TypeInfoImpl<CE_FIRST_ARG(__VA_ARGS__)>\
		{\
			const ClassType Type;\
			const TypeData<CE_FIRST_ARG(__VA_ARGS__)> TypeData;\
		};\
	}\
	template<>\
	inline const TypeInfo* GetStaticType<CE_FIRST_ARG(__VA_ARGS__)>()\
	{\
		static Internal::TypeInfoImpl<CE_FIRST_ARG(__VA_ARGS__)> instance{ ClassType{CE_TOSTRING(CE_FIRST_ARG(__VA_ARGS__))}, TypeData<CE_FIRST_ARG(__VA_ARGS__)>() };\
		return &instance.Type;\
	}\
}

#define CE_RTTI_IMPLEMENT_CLASS(Class)\
const TypeInfo* Class::Type()\
{\
	return CE::template GetStaticType<Self>();\
}


