#pragma once

#include "Misc/CoreDefines.h"

#define CE_CLASS_ATTRIBUTES(...) static constexpr const char* _Attributes = "" #__VA_ARGS__;

#define CE_META_CLASS(nameSpace, className, superClass) friend class CE_Generated_MetaClass_##className##_Singleton;\
typedef className Self;\
typedef superClass Super;\
static constexpr const char* ClassPath = #nameSpace "::" #className;\
static ClassType* Type() {\
	return ClassType::FindClass(ClassPath);\
}\
ClassType* GetType() override {\
	return ClassType::FindClass(ClassPath);\
}



#define CE_EXTRA_SUPER_CLASS(...) #__VA_ARGS__
#define CE_ATTRIBUTES(...) #__VA_ARGS__


#define CE_PROPERTY_LIST(x) x

#define CE_PROPERTY(name, type, baseType, typePath, ...) Properties.Add(new CE::PropertyType(CE::PropertyInitializer{\
    #name, _ClassPath, offsetof(Type, name), sizeof(Type::name), CE::FieldType::type, CE::FieldBaseType::baseType, #typePath, "" #__VA_ARGS__\
}));


#define CE_FUNCTION_LIST(x) x


#define CE_PARAM_LIST(...) __VA_ARGS__
#define CE_PARAM_POS(pos, type) std::any_cast<type>(*(params.begin() + pos))

#define CE_PARAM(position, name, rawTypeName, type, baseType) position, name, rawTypeName, type, baseType

#define CE_PARAM_POSITION_DEFS(position, name, rawTypeName, type, baseType, ...) using name##position = rawTypeName;

#define CE_FUNCTION(name, paramList)\
Functions.Add(new CE::FunctionType(CE::FunctionInitializer{\
    #name, String(_ClassPath) + "::" #name, false, \
    [&](void* instance, std::initializer_list<std::any> params, std::any& result) -> void {\
        Type* ptr = (Type*)instance;\
        result = ptr->name(\
            paramList\
        );\
    },\
    \
    {\
        new CE::ParameterType({ "integer", false, sizeof(s32), CE::FieldType::Plain, CE::FieldBaseType::s32, "CE::s32", "SomeAttribute,Hidden,Max=12" }),\
        new CE::ParameterType({ "extra", false, sizeof(CE::String), CE::FieldType::Plain, CE::FieldBaseType::String, "CE::String", "SomeAttribute,Hidden,Display=Extra Value" }),\
    },\
    \
    new CE::ParameterType({ "", true, sizeof(CE::String), CE::FieldType::Plain, CE::FieldBaseType::String, "", ""})\
}));

//std::any_cast<s32>(*(params.begin() + 0)), \
//std::any_cast<CE::String>(*(params.begin() + 1))\

#define CE_META_DECLARE_CLASS(nameSpace, className, API, superClass, extraSuperClasses, classAttributesMacro, propertyListMacro, functionListMacro)\
class API CE_Generated_MetaClass_##className##_Singleton : public CE::ClassType\
{\
private:\
    CE_Generated_MetaClass_##className##_Singleton();\
    ~CE_Generated_MetaClass_##className##_Singleton();\
\
public:\
    typedef className Type;\
    typedef superClass SuperType;\
    static CE_Generated_MetaClass_##className##_Singleton& Get()\
    {\
        static CE_Generated_MetaClass_##className##_Singleton instance;\
        return instance;\
    }\
private:\
    static constexpr const char* _Name = #className;\
    static constexpr CE::FieldType _Type = CE::FieldType::Plain;\
    static constexpr CE::FieldBaseType _UnderlyingType = CE::FieldBaseType::Class;\
    static constexpr const char* _ClassPath = #nameSpace "::" #className;\
    static constexpr const char* _SuperClasses = "" extraSuperClasses;\
    static constexpr const char* _Attributes = "" classAttributesMacro;\
    void SetupProperties() {\
        propertyListMacro\
    }\
    void SetupFunctions() {\
        functionListMacro\
    }\
};\
API extern CE_Generated_MetaClass_##className##_Singleton& CE_Generated_MetaClass_##className##_Instance;


#define CE_META_IMPLEMENT_CLASS(className, API)\
API CE_Generated_MetaClass_##className##_Singleton& CE_Generated_MetaClass_##className##_Instance \
    = CE_Generated_MetaClass_##className##_Singleton::Get();\
\
CE_Generated_MetaClass_##className##_Singleton::CE_Generated_MetaClass_##className##_Singleton()\
    : CE::ClassType(CE::ClassInitializer{ _Name, sizeof(TestClass), _Type, _UnderlyingType, _ClassPath, _Attributes })\
{\
    SetupProperties();\
    SetupFunctions();\
    ClassType::RegisterClassType(this);\
}\
\
CE_Generated_MetaClass_##className##_Singleton::~CE_Generated_MetaClass_##className##_Singleton()\
{\
    ClassType::UnregisterClassType(this);\
}
