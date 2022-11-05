#pragma once

#include "RTTI.h"
#include "Containers/Array.h"

namespace CE
{

	class StructType : public TypeInfo
	{
	protected:
		StructType(const char* name) : TypeInfo(name)
		{}

		template<typename T>
		friend const TypeInfo* GetStaticType();
        
        template<typename Struct>
        friend struct TypeInfoImpl;

	public:

		virtual bool IsStruct() const override { return true; }
        
    protected:
        
        template<typename Struct, typename Field>
        void AddField(const char* name, Field Struct::* field)
        {
            auto fieldTypeId = CE::GetTypeId<Field>();
        }
        
        CE::Array<String> Fields{};
	};

	class ClassType : public StructType
	{
	protected:
		ClassType(const char* name) : StructType(name)
		{}

		template<typename T>
		friend const TypeInfo* GetStaticType();
        
        template<typename Class>
        friend struct TypeInfoImpl;

	public:

		// TypeData is always located AFTER TypeInfo in memory
		virtual const u8* GetRawTypeData() const override { return (u8*)(this + 1); }

		virtual bool IsStruct() const override { return false; }
		virtual bool IsClass() const override { return true; }
        
    private:
        
	};

}

#define __CE_RTTI_FIELDS(First, ...) First CE_MACRO_EXPAND(__CE_RTTI_FIELDS(__VA_ARGS__))

#define CE_RTTI_FIELDS(...) CE_MACRO_EXPAND(__CE_RTTI_FIELDS(__VA_ARGS__))

#define CE_RTTI_FIELD(FieldName) AddField(#FieldName, &Self::FieldName);

#define __CE_RTTI_JOIN_CLASSES_0()
#define __CE_RTTI_JOIN_CLASSES_1(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_2(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_3(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_4(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_5(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_6(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_7(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_8(...) , __VA_ARGS__

#define __CE_RTTI_JOIN_CLASSES(Class, ...) Class CE_MACRO_EXPAND(CE_CONCATENATE(__CE_RTTI_JOIN_CLASSES_,CE_ARG_COUNT(__VA_ARGS__)))(__VA_ARGS__)

#define CE_RTTI_SUPER(...) __VA_ARGS__

#define CE_RTTI_DECLARE_CLASS(Class, SuperClasses)\
namespace CE\
{\
	template<>\
	struct TypeData<Class> : public Internal::TypeDataImpl<__CE_RTTI_JOIN_CLASSES(Class, SuperClasses)>\
	{\
	};\
	namespace Internal\
	{\
		template<>\
		struct TypeInfoImpl<Class>\
		{\
            typedef Class Self;\
            const CE::ClassType Type;\
			const CE::TypeData<Class> TypeData;\
            TypeInfoImpl(CE::ClassType type, CE::TypeData<Class> typeData) : Type(type), TypeData(typeData)\
            {\
                \
            }\
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
const ClassType* Class::Type()\
{\
	return (ClassType*)(CE::template GetStaticType<Self>());\
}

#define __CE_RTTI_SUPERCLASS_0(...) typedef void Super;
#define __CE_RTTI_SUPERCLASS_1(SuperClass) typedef SuperClass Super;
#define __CE_RTTI_SUPERCLASS_2(SuperClass, ...) __CE_RTTI_SUPERCLASS_1(SuperClass);
#define __CE_RTTI_SUPERCLASS_3(SuperClass, ...) __CE_RTTI_SUPERCLASS_1(SuperClass);

#define __CE_RTTI_SUPERCLASS(...) CE_MACRO_EXPAND(CE_CONCATENATE(__CE_RTTI_SUPERCLASS_, CE_ARG_COUNT(__VA_ARGS__)))(__VA_ARGS__)

#define CE_RTTI_CLASS(Class, ...)\
public:\
    typedef Class Self;\
    __CE_RTTI_SUPERCLASS(__VA_ARGS__)\
    static const ClassType* Type();\
    virtual const TypeInfo* GetType() const\
    {\
        return Type();\
    }

