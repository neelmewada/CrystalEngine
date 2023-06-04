#pragma once

#include "Math.h"
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"

CE_RTTI_POD(CORE_API, CE, Vec2i, TYPEID(CE::Vec3i), TYPEID(CE::Vec4i))
CE_RTTI_POD(CORE_API, CE, Vec3i, TYPEID(CE::Vec2i), TYPEID(CE::Vec4i))
CE_RTTI_POD(CORE_API, CE, Vec4i, TYPEID(CE::Vec2i), TYPEID(CE::Vec3i))

CE_RTTI_POD(CORE_API, CE, Vec2, TYPEID(CE::Vec3), TYPEID(CE::Vec4))
CE_RTTI_POD(CORE_API, CE, Vec3, TYPEID(CE::Vec2), TYPEID(CE::Vec4))
CE_RTTI_POD(CORE_API, CE, Vec4, TYPEID(CE::Vec2), TYPEID(CE::Vec3))

CE_RTTI_POD(CORE_API, CE, Quat)

CE_RTTI_POD(CORE_API, CE, Matrix4x4)

