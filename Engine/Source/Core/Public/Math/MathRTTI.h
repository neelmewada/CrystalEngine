#pragma once

#include "Math.h"
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"


CE_RTTI_POD(CE, Vec2, TYPEID(CE::Vec3), TYPEID(CE::Vec4))
CE_RTTI_POD(CE, Vec3, TYPEID(CE::Vec2), TYPEID(CE::Vec4))
CE_RTTI_POD(CE, Vec4, TYPEID(CE::Vec2), TYPEID(CE::Vec3))

CE_RTTI_POD(CE, Quat)

CE_RTTI_POD(CE, Matrix4x4)
