#pragma once

#include "Misc/CoreDefines.h"

#define CE_META_CLASS(ClassName, ...) friend struct CE_Generated_Meta_##ClassName##_Statics;

#define CE_META_BEGIN_CLASS(Namespace, ClassName, ...)\
struct CE_Generated_Meta_##ClassName##_Statics\
{\
public:

#define CE_META_BEGIN_PROPERTIES(ClassName) using ClassType = ClassName;\
static constexpr PropertyInfo Properties[] = {

#define CE_META_PROPERTY(PropertyName, Type, BaseType, TypePath, ...)\
{ #PropertyName, offsetof(ClassType, PropertyName), sizeof(ClassType::PropertyName), CE::FieldType::Type, CE::FieldBaseType::BaseType, #TypePath, #__VA_ARGS__ },

#define CE_META_END_PROPERTIES() };

#define CE_META_END_CLASS() };

