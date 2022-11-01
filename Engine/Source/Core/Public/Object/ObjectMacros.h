#pragma once

#include "Misc/CoreDefines.h"

#define CE_CLASS_ATTRIBUTES(...) static constexpr const char* _Attributes = "" #__VA_ARGS__;

#define CE_META_CLASS(nameSpace, className, superClass) friend class CE_Generated_MetaClass_##className##_Singleton;\
typedef className Self;\
typedef superClass Super;\
static constexpr const char* ClassPath = #nameSpace "::" #className;


