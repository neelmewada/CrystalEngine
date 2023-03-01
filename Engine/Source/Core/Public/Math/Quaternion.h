#pragma once

#include "Types/CoreTypeDefs.h"

#include "Vector.h"

namespace CE
{
    
    class CORE_API Quat
    {
    public:

        Quat() : x(0), y(0), z(0), w(0)
        {}

        Quat(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w)
        {}

        Quat(Vec3 vec, f32 w) : x(vec.x), y(vec.y), z(vec.z), w(w)
        {}

        static Quat FromEuler(Vec3 euler)
        {
            f32 cr = std::cos(euler.x * 0.5);
            f32 sr = std::sin(euler.x * 0.5);
            f32 cp = std::cos(euler.y * 0.5);
            f32 sp = std::sin(euler.y * 0.5);
            f32 cy = std::cos(euler.z * 0.5);
            f32 sy = std::sin(euler.z * 0.5);

            Quat q;
            q.w = cr * cp * cy + sr * sp * sy;
            q.x = sr * cp * cy - cr * sp * sy;
            q.y = cr * sp * cy + sr * cp * sy;
            q.z = cr * cp * sy - sr * sp * cy;

            return q;
        }

        static Quat FromEuler(f32 x, f32 y, f32 z)
        {
            f32 cr = std::cos(x * 0.5);
            f32 sr = std::sin(x * 0.5);
            f32 cp = std::cos(y * 0.5);
            f32 sp = std::sin(y * 0.5);
            f32 cy = std::cos(z * 0.5);
            f32 sy = std::sin(z * 0.5);

            Quat q;
            q.w = cr * cp * cy + sr * sp * sy;
            q.x = sr * cp * cy - cr * sp * sy;
            q.y = cr * sp * cy + sr * cp * sy;
            q.z = cr * cp * sy - sr * sp * cy;

            return q;
        }

        inline Quat operator+(const Quat& rhs) const
        {
            return Quat(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
        }

        inline Quat operator-(const Quat& rhs) const
        {
            return Quat(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
        }

        inline Quat operator+=(const Quat& rhs)
        {
            *this = *this + rhs;
            return *this;
        }

        inline Quat operator-=(const Quat& rhs)
        {
            *this = *this - rhs;
            return *this;
        }

        inline Quat operator*(const Quat& rhs)
        {
            return Multiply(rhs);
        }

        inline Quat operator*(s32 rhs)
        {
            return Quat(x * rhs, y * rhs, z * rhs, w * rhs);
        }

        inline Quat operator*(f32 rhs)
        {
            return Quat(x * rhs, y * rhs, z * rhs, w * rhs);
        }

        inline Quat operator/(s32 rhs)
        {
            return Quat(x / rhs, y / rhs, z / rhs, w / rhs);
        }

        inline Quat operator/(f32 rhs)
        {
            return Quat(x / rhs, y / rhs, z / rhs, w / rhs);
        }

        inline Quat operator*=(s32 rhs)
        {
            *this = Quat(x * rhs, y * rhs, z * rhs, w * rhs);
            return *this;
        }

        inline Quat operator*=(f32 rhs)
        {
            *this = Quat(x * rhs, y * rhs, z * rhs, w * rhs);
            return *this;
        }

        Vec3 operator*(const Vec3& rhs)
        {
            // q' = q * p * (q^-1) ; q is unit norm of rotation quaternion

            // Convert vector to pure quaternion
            Quat p = Quat(rhs, 0);

            Quat q = GetUnitNorm();
            Quat qInverse = q.GetInverse();

            Quat result = q * p * qInverse;
            return result.GetVectorComponent();
        }

        Vec3 ToEuler() const
        {
            Vec3 angles{};

            // roll (x-axis rotation)
            f32 sinr_cosp = 2 * (w * x + y * z);
            f32 cosr_cosp = 1 - 2 * (x * x + y * y);
            angles.x = std::atan2(sinr_cosp, cosr_cosp);

            // pitch (y-axis rotation)
            f32 sinp = std::sqrt(1 + 2 * (w * x - y * z));
            f32 cosp = std::sqrt(1 - 2 * (w * x - y * z));
            angles.y = 2 * std::atan2(sinp, cosp) - M_PI / 2;

            // yaw (z-axis rotation)
            f32 siny_cosp = 2 * (w * z + x * y);
            f32 cosy_cosp = 1 - 2 * (y * y + z * z);
            angles.z = std::atan2(siny_cosp, cosy_cosp);

            return angles;
        }
        
        inline f32 GetSqrNorm()
        {
            return x * x + y * y + z * z + w * w;
        }

        inline f32 GetNorm()
        {
            return Math::Sqrt(GetSqrNorm());
        }

        inline Quat GetNormalized()
        {
            return *this * (1 / GetNorm());
        }

        inline Vec3 GetVectorComponent()
        {
            return Vec3(x, y, z);
        }

        Quat GetUnitNorm()
        {
            f32 angle = Math::ToRadians(w);
            auto normalizedVec = Vec3(x, y, z).GetNormalized();
            w = Math::Cos(angle * 0.5f);
            normalizedVec = normalizedVec * Math::Sin(angle * 0.5f);

            return Quat(normalizedVec.x, normalizedVec.y, normalizedVec.z, w);
        }

        Quat GetUnitNormLerped(f32 t)
        {
            f32 angle = Math::ToRadians(w);
            auto normalizedVec = Vec3(x, y, z).GetNormalized();
            w = Math::Cos(angle * Math::Clamp01(t) * 0.5f);
            normalizedVec = normalizedVec * Math::Sin(angle * Math::Clamp01(t) * 0.5f);

            return Quat(normalizedVec.x, normalizedVec.y, normalizedVec.z, w);
        }

        inline void Normalize()
        {
            auto norm = GetNorm();

            if (norm != 0)
            {
                f32 normValue = 1 / norm;

                *this *= normValue;
            }
        }

        inline Quat GetConjugate()
        {
            return Quat(-x, -y, -z, w);
        }

        inline Quat GetInverse()
        {
            return GetConjugate() / GetSqrNorm();
        }

        Quat Multiply(const Quat& quat);

        inline static Quat Lerp(Quat from, Quat to, f32 t)
        {
            return from * (1 - t) + to * t;
        }

        // TODO: To be tested later
        static Quat Slerp(Quat from, Quat to, f32 t)
        {
            return (to * from.GetInverse()).GetUnitNormLerped(t) * from;
        }

    public:

        union {
            struct {
                f32 x, y, z, w;
            };

            f32 xyzw[4];
        };
    };

    inline Quat operator*(s32 lhs, const Quat& rhs)
    {
        return Quat(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
    }

    inline Quat operator*(f32 lhs, const Quat& rhs)
    {
        return Quat(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
    }

} // namespace CE

CE_RTTI_POD(CE, Quat)
