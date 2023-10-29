#pragma once

#include "Types/CoreTypeDefs.h"

namespace CE
{
	class Matrix4x4;
    
    class CORE_API Quat
    {
    public:

		Quat();
		Quat(float w, float x, float y, float z);
		Quat(const Quat& q);
		~Quat();
		Quat& operator=(const Quat&);

		bool operator==(const Quat& rhs) const;
		bool operator!=(const Quat& rhs) const;

		Quat& operator*=(const Quat& rhs);
		Quat operator*(const Quat& rhs) const;
		Quat operator*(float rhs) const;
		Quat operator+(const Quat& rhs) const;
		Quat operator-(const Quat& rhs) const;
		Vec4 operator*(const Vec4& rhs) const;
		Vec3 operator*(const Vec3& rhs) const;

		Quat GetNormalized();
		void Normalize();
		Quat Conjugate(); // Same as inverse

		void SetEulerDegrees(float X, float Y, float Z);
		float Dot(const Quat& b) const;

		Matrix4x4 ToMatrix() const;

		static Quat LookRotation(Vec4& lookAt);
		static Quat LookRotation(Vec4& lookAt, Vec4& upDirection);
		static Quat Slerp(Quat& from, Quat& to, float t);
		static Quat Lerp(Quat& from, Quat& to, float t);
		static float Angle(Quat& a, Quat& b);
		static float Dot(const Quat& a, const Quat& b);
		static Quat AngleAxis(float angle, Vec4& axis);
		static Quat Inverse(Quat& rotation);
		static Quat EulerDegrees(float x, float y, float z);
		static Quat EulerRadians(float x, float y, float z);
		static Matrix4x4 ToMatrix(const Quat& q);

		inline static Quat EulerDegrees(Vec3 euler)
		{
			return EulerDegrees(euler.x, euler.y, euler.z);
		}

		inline static Quat EulerRadians(Vec3 euler)
		{
			return EulerRadians(euler.x, euler.y, euler.z);
		}

    public:

        union {
            struct {
                f32 x, y, z, w;
            };

            f32 xyzw[4];
        };
    };

	Vec4 operator*(const Vec4& v, const Quat& m);
	Vec3 operator*(const Vec3& v, const Quat& m);
	Quat operator*(float f, const Quat& m);

} // namespace CE


