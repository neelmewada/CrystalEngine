
#include "Math/Vector.h"
#include "Math/Quaternion.h"


namespace CE
{

    

	Quat::Quat()
	{
	}

	Quat::Quat(float w, float x, float y, float z) : w(w), x(x), y(y), z(z)
	{
	}

	Quat::Quat(const Quat& q)
	{
		w = q.w;
		x = q.x;
		y = q.y;
		z = q.z;
	}

	Quat::~Quat()
	{
	}

	Quat& Quat::operator=(const Quat& q)
	{
		if (this == &q)
			return *this;
		w = q.w;
		x = q.x;
		y = q.y;
		z = q.z;
		return *this;
	}

	bool Quat::operator==(const Quat& rhs) const
	{
		return w == rhs.w && x == rhs.x && y == rhs.y && z == rhs.z;
	}

	bool Quat::operator!=(const Quat& rhs) const
	{
		return !(*this == rhs);
	}

	Quat& Quat::operator*=(const Quat& rhs)
	{
		Quat q;

		q.w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
		q.x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
		q.y = w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x;
		q.z = w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w;

		*this = q;
		return *this;
	}

	Quat Quat::operator*(const Quat& rhs) const
	{
		Quat q;

		q.w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
		q.x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
		q.y = w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x;
		q.z = w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w;

		return q;
	}

	Quat Quat::operator*(float rhs) const
	{
		return Quat(w * rhs, x * rhs, y * rhs, z * rhs);
	}

	Quat Quat::operator+(const Quat& rhs) const
	{
		return Quat(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Quat Quat::operator-(const Quat& rhs) const
	{
		return Quat(w - rhs.w, x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Vec4 Quat::operator*(const Vec4& rhs) const
	{
		return Quat::ToMatrix(*this) * rhs;
	}

	Vec3 Quat::operator*(const Vec3& rhs) const
	{
		return Quat::ToMatrix(*this) * Vec4(rhs, 1.0f);
	}

	Quat Quat::GetNormalized()
	{
		float mag = sqrtf(w * w + x * x + y * y + z * z);
		return Quat(w / mag, x / mag, y / mag, z / mag);
	}

	void Quat::Normalize()
	{
		float mag = sqrtf(w * w + x * x + y * y + z * z);

		w /= mag;
		x /= mag;
		y /= mag;
		z /= mag;
	}

	Quat Quat::Conjugate()
	{
		return Quat(w, -x, -y, -z);
	}

	void Quat::SetEulerDegrees(float x, float y, float z)
	{
		this->x *= 0.0174532925f; // To radians!
		this->y *= 0.0174532925f; // To radians!
		this->z *= 0.0174532925f; // To radians!

		this->x *= 0.5f;
		this->y *= 0.5f;
		this->z *= 0.5f;
		
		float sinx = sinf(this->x);
		float siny = sinf(this->y);
		float sinz = sinf(this->z);
		float cosx = cosf(this->x);
		float cosy = cosf(this->y);
		float cosz = cosf(this->z);

		w = cosx * cosy * cosz + sinx * siny * sinz;
		x = sinx * cosy * cosz + cosx * siny * sinz;
		y = cosx * siny * cosz - sinx * cosy * sinz;
		z = cosx * cosy * sinz - sinx * siny * cosz;
	}

	float Quat::Dot(const Quat& b) const
	{
		return (w * b.w + x * b.x * y * b.y + z * b.z);
	}

	Matrix4x4 Quat::ToMatrix() const
	{
		return ToMatrix(*this);
	}

	Quat Quat::LookRotation(Vec4& lookAt)
	{
		return Quat();
	}

	Quat Quat::LookRotation(Vec4& lookAt, Vec4& upDirection)
	{
		return Quat();
	}

	Quat Quat::Slerp(Quat& from, Quat& to, float t)
	{
		float cosTheta = Quat::Dot(from, to);
		Quat temp(to);

		if (cosTheta < 0.0f) {
			cosTheta *= -1.0f;
			temp = temp * -1.0f;
		}

		float theta = acosf(cosTheta);
		float sinTheta = 1.0f / sinf(theta);

		return sinTheta * (
			((Quat)(from * sinf(theta * (1.0f - t)))) +
			((Quat)(temp * sinf(t * theta)))
			);
	}

	Quat Quat::Lerp(Quat& from, Quat& to, float t)
	{
		Quat src = from * (1.0f - t);
		Quat dst = to * t;

		Quat q = src + dst;
		return q;
	}

	float Quat::Angle(Quat& a, Quat& b)
	{
		float degrees = acosf((b * Quat::Inverse(a)).w) * 2.0f * 57.2957795f;
		if (degrees > 180.0f)
			return 360.0f - degrees;
		return degrees;
	}

	float Quat::Dot(const Quat& a, const Quat& b)
	{
		return a.Dot(b);
	}

	Quat Quat::AngleAxis(float angle, Vec4& axis)
	{
		Vec4 vn = axis.GetNormalized();

		angle *= 0.0174532925f; // To radians!
		angle *= 0.5f;
		float sinAngle = sin(angle);

		return Quat(cos(angle), vn.x * sinAngle, vn.y * sinAngle, vn.z * sinAngle);
	}

	Quat Quat::Inverse(Quat& rotation)
	{
		return Quat(rotation.w, -1.0f * rotation.x, -1.0f * rotation.y, -1.0f * rotation.z);
	}

	Quat Quat::EulerDegrees(float X, float Y, float Z)
	{
		X *= 0.0174532925f; // To radians!
		Y *= 0.0174532925f; // To radians!
		Z *= 0.0174532925f; // To radians!

		return EulerRadians(X, Y, Z);
	}

	Quat Quat::EulerRadians(float X, float Y, float Z)
	{
		X *= 0.5f;
		Y *= 0.5f;
		Z *= 0.5f;

		float sinx = sinf(X);
		float siny = sinf(Y);
		float sinz = sinf(Z);
		float cosx = cosf(X);
		float cosy = cosf(Y);
		float cosz = cosf(Z);

		Quat q;

		q.w = cosx * cosy * cosz + sinx * siny * sinz;
		q.x = sinx * cosy * cosz + cosx * siny * sinz;
		q.y = cosx * siny * cosz - sinx * cosy * sinz;
		q.z = cosx * cosy * sinz - sinx * siny * cosz;

		return q;
	}

	Matrix4x4 Quat::ToMatrix(const Quat& q)
	{
		float sqw = q.w * q.w;
		float sqx = q.x * q.x;
		float sqy = q.y * q.y;
		float sqz = q.z * q.z;
		float invs = 1.0f / (sqx + sqy + sqz + sqw);

		Matrix4x4 matrix = Matrix4x4::Identity();

		f32* mat = &matrix.rows[0][0];

		mat[0] = (sqx - sqy - sqz + sqw) * invs;
		mat[5] = (-sqx + sqy - sqz + sqw) * invs;
		mat[10] = (-sqx - sqy + sqz + sqw) * invs;

		float tmp1 = q.x * q.y;
		float tmp2 = q.z * q.w;
		mat[4] = 2.0 * (tmp1 + tmp2) * invs;
		mat[1] = 2.0 * (tmp1 - tmp2) * invs;

		tmp1 = q.x * q.z;
		tmp2 = q.y * q.w;
		mat[8] = 2.0 * (tmp1 - tmp2) * invs;
		mat[2] = 2.0 * (tmp1 + tmp2) * invs;

		tmp1 = q.y * q.z;
		tmp2 = q.x * q.w;
		mat[9] = 2.0 * (tmp1 + tmp2) * invs;
		mat[6] = 2.0 * (tmp1 - tmp2) * invs;

		return matrix;
	}

	Vec4 operator*(const Vec4& v, const Quat& m)
	{
		return Quat::ToMatrix(m) * v;
	}

	Vec3 operator*(const Vec3& v, const Quat& m)
	{
		return Quat::ToMatrix(m) * Vec4(v, 1.0f);
	}

	Quat operator*(float f, const Quat& m)
	{
		return Quat(m.w * f, m.x * f, m.y * f, m.z * f);
	}

} // namespace CE
