#pragma once


#define __CE_SUPER_LIST(...) CE_EXPAND(CE_CONCATENATE(__CE_SUPER_LIST_,CE_ARG_COUNT(__VA_ARGS__)))(__VA_ARGS__)

#define CE_FIELD_LIST(x) x
#define CE_FIELD(FieldName, ...) Type.AddField(#FieldName, &Self::FieldName, offsetof(Self, FieldName), "" #__VA_ARGS__);

#define CE_FUNCTION_LIST(x) x
#define CE_FUNCTION(FunctionName, ...) Type.AddFunction(#FunctionName, &Self::FunctionName, "" #__VA_ARGS__);

#define CE_FUNCTION2(FunctionName, ReturnType, Signature, ...)\
{\
	ReturnType (Self::*funcPtr)Signature = &Self::FunctionName;\
	Type.AddFunction(#FunctionName, funcPtr, "" #__VA_ARGS__);\
}

#define __CE_RTTI_JOIN_CLASSES_0()
#define __CE_RTTI_JOIN_CLASSES_1(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_2(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_3(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_4(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_5(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_6(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_7(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_8(...) , __VA_ARGS__
#define __CE_RTTI_JOIN_CLASSES_9(...) , __VA_ARGS__

#define __CE_RTTI_JOIN_CLASSES(Class, ...) Class CE_EXPAND(CE_CONCATENATE(__CE_RTTI_JOIN_CLASSES_,CE_ARG_COUNT(__VA_ARGS__)))(__VA_ARGS__)

#define CE_SUPER(...) __VA_ARGS__
#define CE_ATTRIBS(...) #__VA_ARGS__

#define __CE_NEW_INSTANCE_(Namespace, Class) new Namespace::Class
#define __CE_NEW_INSTANCE_NotAbstract(Namespace, Class) new Namespace::Class
#define __CE_NEW_INSTANCE_false(Namespace, Class) new Namespace::Class
#define __CE_NEW_INSTANCE_Abstract(Namespace, Class) nullptr
#define __CE_NEW_INSTANCE_true(Namespace, Class) nullptr

#define __CE_CAN_INSTANTIATE_() true
#define __CE_CAN_INSTANTIATE_Abstract() false
#define __CE_CAN_INSTANTIATE_NotAbstract() true
#define __CE_CAN_INSTANTIATE_false() true
#define __CE_CAN_INSTANTIATE_true() false

#define __CE_INIT_DEFAULTS_(Namespace, Class) new(instance) Namespace::Class
#define __CE_INIT_DEFAULTS_NotAbstract(Namespace, Class) new(instance) Namespace::Class
#define __CE_INIT_DEFAULTS_false(Namespace, Class) new(instance) Namespace::Class
#define __CE_INIT_DEFAULTS_Abstract(Namespace, Class)
#define __CE_INIT_DEFAULTS_true(Namespace, Class)

#define CE_ABSTRACT Abstract
#define CE_NOT_ABSTRACT NotAbstract
#define CE_DONT_INSTANTIATE Abstract
#define CE_INSTANTIATE NotAbstract

#define CE_RTTI_CLASS(API, Namespace, Class, SuperClasses, IsAbstract, Attributes, FieldList, FunctionList)\
namespace CE\
{\
	template<>\
	struct StructTypeData<Namespace::Class> : public Internal::TypeDataImpl<__CE_RTTI_JOIN_CLASSES(Namespace::Class, SuperClasses)>\
	{};\
	namespace Internal\
	{\
		template<>\
		struct TypeInfoImpl<Namespace::Class> : public CE::Internal::IClassTypeImpl\
		{\
            typedef Namespace::Class Self;\
            CE::ClassType Type;\
			const CE::StructTypeData<Namespace::Class> TypeData;\
            TypeInfoImpl(const char* fullName, Internal::IClassTypeImpl* impl, u32 size, CE::StructTypeData<Namespace::Class> typeData)\
				: Type(fullName, impl, size, Attributes "")\
				, TypeData(typeData)\
            {\
				Type.AddSuper<SuperClasses>();\
				FieldList\
				FunctionList\
            }\
			virtual ~TypeInfoImpl()\
			{\
			}\
			virtual bool CanInstantiate() const override\
			{\
				return CE_EXPAND(CE_CONCATENATE(__CE_CAN_INSTANTIATE_,CE_FIRST_ARG(IsAbstract)))();\
			}\
			virtual Object* CreateInstance() const override\
			{\
				return CE_EXPAND(CE_CONCATENATE(__CE_NEW_INSTANCE_,CE_FIRST_ARG(IsAbstract)))(Namespace, Class);\
			}\
			virtual void InitializeDefaults(void* instance) const override\
			{\
				CE_EXPAND(CE_CONCATENATE(__CE_INIT_DEFAULTS_,CE_FIRST_ARG(IsAbstract)))(Namespace, Class);\
			}\
		};\
	}\
	template<>\
	inline TypeInfo* GetStaticType<Namespace::Class>()\
	{\
		return Namespace::Class::Type();\
	}\
	template<>\
	inline CE::Name GetTypeName<Namespace::Class>()\
	{\
		static Name name = Name(#Namespace "::" #Class);\
		return name;\
	}\
	template<>\
	inline ClassType* GetStaticClass<Namespace::Class>()\
	{\
		return (ClassType*)GetStaticType<Namespace::Class>();\
	}\
}

#define CE_RTTI_CLASS_IMPL(API, Namespace, Class)\
CE::ClassType* Namespace::Class::Type()\
{\
	static Internal::TypeInfoImpl<Namespace::Class> instance{\
			#Namespace "::" #Class,\
			&instance, sizeof(Namespace::Class),\
			StructTypeData<Namespace::Class>()\
	};\
	return &instance.Type;\
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
    static CE::ClassType* Type();\
    virtual const CE::TypeInfo* GetType() const\
    {\
        return Type();\
    }\
	virtual CE::ClassType* GetClass() const\
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
		struct TypeInfoImpl<Namespace::Struct> : public CE::Internal::IStructTypeImpl\
		{\
            typedef Namespace::Struct Self;\
            CE::StructType Type;\
			const CE::StructTypeData<Namespace::Struct> TypeData;\
            TypeInfoImpl(CE::StructType&& type, CE::StructTypeData<Namespace::Struct> typeData) : Type(type), TypeData(typeData)\
            {\
				Type.AddSuper<SuperStructs>();\
				FieldList\
            }\
			virtual ~TypeInfoImpl()\
			{\
			}\
			virtual void InitializeDefaults(void* instance) const override\
			{\
				new(instance) Namespace::Struct;\
			}\
		};\
	}\
	template<>\
	inline TypeInfo* GetStaticType<Namespace::Struct>()\
	{\
        static Internal::TypeInfoImpl<Namespace::Struct> instance{ StructType{ #Namespace "::" #Struct, &instance, sizeof(Namespace::Struct), Attributes "" }, StructTypeData<Namespace::Struct>() };\
		return &instance.Type;\
	}\
	template<>\
	inline CE::Name GetTypeName<Namespace::Struct>()\
	{\
		static Name name = Name(#Namespace "::" #Struct);\
		return name;\
	}\
	template<>\
	inline StructType* GetStaticStruct<Namespace::Struct>()\
	{\
		return (StructType*)GetStaticType<Namespace::Struct>();\
	}\
}

#define CE_RTTI_STRUCT_IMPL(API, Namespace, Struct)\
CE::StructType* Namespace::Struct::Type()\
{\
	return (CE::StructType*)(CE::template GetStaticType<Self>());\
}

#define CE_STRUCT(Struct, ...)\
public:\
	template<typename T>\
	friend struct CE::Internal::TypeInfoImpl;\
    typedef Struct Self;\
    __CE_RTTI_SUPERCLASS(__VA_ARGS__)\
    static CE::StructType* Type();\
    virtual CE::TypeInfo* GetType() const\
    {\
        return Type();\
    }\
	virtual CE::StructType* GetStruct() const\
	{\
		return Type();\
	}

