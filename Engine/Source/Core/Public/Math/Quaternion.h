#pragma once

#include "Types/CoreTypeDefs.h"

#include "Vector.h"

namespace CE
{
    
    class CORE_API Quat
    {
    public:

        Quat() : X(0), Y(0), Z(0), W(0)
        {}

        Quat(f32 x, f32 y, f32 z, f32 w) : X(x), Y(y), Z(z), W(w)
        {}

        Quat(Vec3 vec, f32 w) : X(vec.X), Y(vec.Y), Z(vec.Z), W(w)
        {}

        inline Quat operator+(const Quat& rhs) const
        {
            return Quat(X + rhs.X, Y + rhs.Y, Z + rhs.Z, W + rhs.W);
        }

        inline Quat operator-(const Quat& rhs) const
        {
            return Quat(X - rhs.X, Y - rhs.Y, Z - rhs.Z, W - rhs.W);
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
            return Quat(X * rhs, Y * rhs, Z * rhs, W * rhs);
        }

        inline Quat operator*(f32 rhs)
        {
            return Quat(X * rhs, Y * rhs, Z * rhs, W * rhs);
        }

        inline Quat operator/(s32 rhs)
        {
            return Quat(X / rhs, Y / rhs, Z / rhs, W / rhs);
        }

        inline Quat operator/(f32 rhs)
        {
            return Quat(X / rhs, Y / rhs, Z / rhs, W / rhs);
        }

        inline Quat operator*=(s32 rhs)
        {
            *this = Quat(X * rhs, Y * rhs, Z * rhs, W * rhs);
            return *this;
        }

        inline Quat operator*=(f32 rhs)
        {
            *this = Quat(X * rhs, Y * rhs, Z * rhs, W * rhs);
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

        inline f32 GetSqrNorm()
        {
            return X * X + Y * Y + Z * Z + W * W;
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
            return Vec3(X, Y, Z);
        }

        Quat GetUnitNorm()
        {
            f32 angle = Math::ToRadians(W);
            auto normalizedVec = Vec3(X, Y, Z).GetNormalized();
            W = Math::Cos(angle * 0.5f);
            normalizedVec = normalizedVec * Math::Sin(angle * 0.5f);

            return Quat(normalizedVec.X, normalizedVec.Y, normalizedVec.Z, W);
        }

        Quat GetUnitNormLerped(f32 t)
        {
            f32 angle = Math::ToRadians(W);
            auto normalizedVec = Vec3(X, Y, Z).GetNormalized();
            W = Math::Cos(angle * Math::Clamp01(t) * 0.5f);
            normalizedVec = normalizedVec * Math::Sin(angle * Math::Clamp01(t) * 0.5f);

            return Quat(normalizedVec.X, normalizedVec.Y, normalizedVec.Z, W);
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
            return Quat(-X, -Y, -Z, W);
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
                f32 X, Y, Z, W;
            };

            f32 XYZW[4];
        };
    };

    inline Quat operator*(s32 lhs, const Quat& rhs)
    {
        return Quat(lhs * rhs.X, lhs * rhs.Y, lhs * rhs.Z, lhs * rhs.W);
    }

    inline Quat operator*(f32 lhs, const Quat& rhs)
    {
        return Quat(lhs * rhs.X, lhs * rhs.Y, lhs * rhs.Z, lhs * rhs.W);
    }

} // namespace CE

CE_RTTI_POD(CE, Quat)
