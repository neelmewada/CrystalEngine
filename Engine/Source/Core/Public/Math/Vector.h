#pragma once

namespace CE
{
    template<typename T>
    class TVector2;

    template<typename T>
    class TVector3;

    template<typename T>
    class TVector4;

    typedef TVector2<f32> Vec2;
    typedef TVector3<f32> Vec3;
    typedef TVector4<f32> Vec4;

    typedef TVector2<s32> Vec2i;
    typedef TVector3<s32> Vec3i;
    typedef TVector4<s32> Vec4i;

	typedef Vec2 Range;
	typedef Vec2i RangeInt;

    inline bool ApproxEquals(f32 lhs, f32 rhs)
    {
        return std::abs(lhs - rhs) < std::numeric_limits<f32>::epsilon();
    }

    template<typename T>
    class TVector2
    {
    public:

        TVector2() : x(0), y(0)
        {}

        TVector2(T x, T y) : x(x), y(y)
        {}

        TVector2(TVector3<T> vec3);
        TVector2(TVector4<T> vec4);

        union {
            struct {
                T x, y;
            };
			struct {
				T min, max;
			};
			struct {
				T left, right;
			};
			struct {
				T top, bottom;
			};
			struct {
				T width, height;
			};
            T xy[2];
        };

		FORCE_INLINE SIZE_T GetHash() const
		{
			return GetCombinedHashes({
				CE::GetHash(x), CE::GetHash(y)
			});
		}

        inline TVector2 operator+(const TVector2& rhs) const
        {
            return TVector2(x + rhs.x, y + rhs.y);
        }

        inline TVector2 operator-(const TVector2& rhs) const
        {
            return TVector2(x - rhs.x, y - rhs.y);
        }

        inline TVector2 operator+() const
        {
            return *this;
        }

        inline TVector2 operator-() const
        {
            return TVector2(-x, -y);
        }

        inline TVector2 operator+=(const TVector2& rhs)
        {
            *this = *this + rhs;
            return *this;
        }

        inline TVector2 operator-=(const TVector2& rhs)
        {
            *this = *this - rhs;
            return *this;
        }

        inline bool operator==(const TVector2& rhs) const
        {
            return x == rhs.x && y == rhs.y;
        }

        inline bool operator!=(const TVector2& rhs) const
        {
            return x != rhs.x || y != rhs.y;
        }

        inline TVector2 operator*(s32 value) const
        {
            return TVector2(value * x, value * y);
        }

        inline TVector2 operator*(u32 value) const
        {
            return TVector2(value * x, value * y);
        }

        inline TVector2 operator*(f32 value) const
        {
            return TVector2(value * x, value * y);
        }

        inline TVector2 operator*=(s32 value)
        {
            *this = *this * value;
            return *this;
        }

        inline TVector2 operator*=(u32 value)
        {
            *this = *this * value;
            return *this;
        }

        inline TVector2 operator*=(f32 value)
        {
            *this = *this * value;
            return *this;
        }

        inline TVector2 operator/(f32 value) const
        {
            return TVector2(x / value, y / value);
        }

        inline TVector2 operator/=(f32 value)
        {
            *this = *this / value;
            return *this;
        }

        inline f32 GetSqrMagnitude() const
        {
            return x * x + y * y;
        }

        inline f32 GetMagnitude() const
        {
            return sqrt(GetSqrMagnitude());
        }

        inline TVector2 GetNormalized() const
        {
            return *this / GetMagnitude();
        }

        inline static T Dot(TVector2 a, TVector2 b)
        {
            return a.x * b.x + a.y * b.y;
        }

        inline f32 Dot(TVector2 b) const
        {
            return x * b.x + y * b.y;
        }

        /// Signed angle in radians between 2 vectors
        inline static f32 SignedAngle(TVector2 a, TVector2 b)
        {
            return Math::ACos(Dot(a, b) / (a.GetMagnitude() * b.GetMagnitude())); // TODO
        }

        /// Signed angle in radians between 2 vectors
        inline f32 SignedAngle(TVector2 b) const
        {
            return SignedAngle(*this, b);
        }

        inline static TVector2<f32> Lerp(TVector2 from, TVector2 to, f32 t)
        {
            return TVector2<f32>(Math::Lerp(from.x, to.x, t), Math::Lerp(from.y, to.y, t));
        }

        inline static TVector2<f32> LerpUnclamped(TVector2 from, TVector2 to, f32 t)
        {
            return TVector2<f32>(Math::LerpUnclamped(from.x, to.x, t), Math::LerpUnclamped(from.y, to.y, t));
        }

        inline String ToString() const
        {
            return String::Format("({}, {})", x, y);
        }
    };
    
    template<typename T>
    class TVector3
    {
    public:

        TVector3() : x(0), y(0), z(0)
        {}

        TVector3(T x, T y) : x(x), y(y), z(0)
        {}

        TVector3(T x, T y, T z) : x(x), y(y), z(z)
        {}

        TVector3(TVector2<T> vec2);
        TVector3(TVector4<T> vec4);

        union {
            struct {
                T x, y, z;
            };
            T xyz[4]; // size/alignment is same as Vector4
        };

		FORCE_INLINE SIZE_T GetHash() const
		{
			return GetCombinedHashes({
				CE::GetHash(x), CE::GetHash(y), CE::GetHash(z)
			});
		}

        inline T operator[](int index) const
        {
            return xyz[index];
        }

        inline TVector3 operator+(const TVector3& rhs) const
        {
            return TVector3(x + rhs.x, y + rhs.y, z + rhs.z);
        }

        inline TVector3 operator-(const TVector3& rhs) const
        {
            return TVector3(x - rhs.x, y - rhs.y, z - rhs.z);
        }

        inline TVector3 operator+() const
        {
            return *this;
        }

        inline TVector3 operator-() const
        {
            return TVector3(-x, -y, -z);
        }

        inline TVector3 operator+=(const TVector3& rhs)
        {
            *this = *this + rhs;
            return *this;
        }

        inline TVector3 operator-=(const TVector3& rhs)
        {
            *this = *this - rhs;
            return *this;
        }

        inline bool operator==(const TVector3& rhs) const
        {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        inline bool operator!=(const TVector3& rhs) const
        {
            return x != rhs.x || y != rhs.y || z != rhs.z;
        }

        inline TVector3 operator*(s32 value) const
        {
            return TVector3(value * x, value * y, value * z);
        }

        inline TVector3 operator*(u32 value) const
        {
            return TVector3(value * x, value * y, value * z);
        }

        inline TVector3 operator*(f32 value) const
        {
            return TVector3(value * x, value * y, value * z);
        }

        inline TVector3 operator*=(s32 value)
        {
            *this = *this * value;
            return *this;
        }

        inline TVector3 operator*=(u32 value)
        {
            *this = *this * value;
            return *this;
        }

        inline TVector3 operator*=(f32 value)
        {
            *this = *this * value;
            return *this;
        }

        inline TVector3 operator/(f32 value) const
        {
            return TVector3(x / value, y / value, z / value);
        }

        inline TVector3 operator/=(f32 value)
        {
            *this = *this / value;
            return *this;
        }

        inline f32 GetSqrMagnitude() const
        {
            return x * x + y * y + z * z;
        }

        inline f32 GetMagnitude() const
        {
            return sqrt(GetSqrMagnitude());
        }

        inline TVector3 GetNormalized() const
        {
            return *this / GetMagnitude();
        }

        inline static T Dot(TVector3 a, TVector3 b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        inline f32 Dot(TVector3 b) const
        {
            return x * b.x + y * b.y + z * b.z;
        }

        inline static TVector3 Cross(TVector3 a, TVector3 b)
        {
            return TVector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
        }

        inline TVector3 Cross(TVector3 b) const
        {
            return TVector3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
        }

        /// Signed angle in radians between 2 vectors
        inline static f32 SignedAngle(TVector3 a, TVector3 b)
        {
            return Math::ACos(Dot(a, b) / (a.GetMagnitude() * b.GetMagnitude())); // TODO
        }

        /// Signed angle in radians between 2 vectors
        inline f32 SignedAngle(TVector3 b) const
        {
            return SignedAngle(*this, b);
        }

        inline static TVector3<f32> Lerp(TVector3 from, TVector3 to, f32 t)
        {
            return TVector3<f32>(Math::Lerp(from.x, to.x, t), Math::Lerp(from.y, to.y, t), Math::Lerp(from.z, to.z, t));
        }

        inline static TVector3<f32> LerpUnclamped(TVector3 from, TVector3 to, f32 t)
        {
            return TVector3<f32>(Math::LerpUnclamped(from.x, to.x, t), Math::LerpUnclamped(from.y, to.y, t), Math::LerpUnclamped(from.z, to.z, t));
        }

        inline String ToString() const
        {
            return String::Format("({}, {}, {})", x, y, z);
        }
    };

    template<typename T>
    class TVector4
    {
    public:

        TVector4() : x(0), y(0), z(0), w(0)
        {}

        TVector4(T x, T y) : x(x), y(y), z(0), w(0)
        {}

        TVector4(T x, T y, T z) : x(x), y(y), z(z), w(0)
        {}

		TVector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w)
		{}

        TVector4(TVector2<T> vec2);
		TVector4(TVector2<T> xy, TVector2<T> zw);
        TVector4(TVector3<T> vec3);
        
        TVector4(TVector3<T> vec3, T w) : x(vec3.x), y(vec3.y), z(vec3.z), w(w)
        {}

        union {
            struct {
                T x, y, z, w;
            };
			struct {
				T left, top, right, bottom;
			};
			struct {
				T topLeft, topRight, bottomRight, bottomLeft;
			};
			struct {
				TVector2<T> min, max;
			};
			struct {
				TVector2<T> pos, size;
			};
            T xyzw[4];
        };

		FORCE_INLINE SIZE_T GetHash() const
		{
			return GetCombinedHashes({
				CE::GetHash(x), CE::GetHash(y), CE::GetHash(z), CE::GetHash(w)
			});
		}

        inline T& operator[](int index)
        {
            return xyzw[index];
        }

        inline const T& operator[](int index) const
        {
            return xyzw[index];
        }

        inline TVector4 operator+(const TVector4& rhs) const
        {
            return TVector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
        }

        inline TVector4 operator-(const TVector4& rhs) const
        {
            return TVector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
        }

        inline TVector4 operator+() const
        {
            return *this;
        }

        inline TVector4 operator-() const
        {
            return TVector4(-x, -y, -z, -w);
        }

        inline TVector4 operator+=(const TVector4& rhs)
        {
            *this = *this + rhs;
            return *this;
        }

        inline TVector4 operator-=(const TVector4& rhs)
        {
            *this = *this - rhs;
            return *this;
        }

        inline bool operator==(const TVector4& rhs) const
        {
            return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
        }

        inline bool operator!=(const TVector4& rhs) const
        {
            return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
        }

        inline TVector4 operator*(s32 value) const
        {
            return TVector4(value * x, value * y, value * z, value * w);
        }

        inline TVector4 operator*(u32 value) const
        {
            return TVector4(value * x, value * y, value * z, value * w);
        }

        inline TVector4 operator*(f32 value) const
        {
            return TVector4(value * x, value * y, value * z, value * w);
        }

        inline TVector4 operator*=(s32 value)
        {
            *this = *this * value;
            return *this;
        }

        inline TVector4 operator*=(u32 value)
        {
            *this = *this * value;
            return *this;
        }

        inline TVector4 operator*=(f32 value)
        {
            *this = *this * value;
            return *this;
        }

        inline TVector4 operator/(f32 value) const
        {
            return TVector4(x / value, y / value, z / value, w / value);
        }

        inline TVector4 operator/=(f32 value)
        {
            *this = *this / value;
            return *this;
        }

        inline f32 GetSqrMagnitude() const
        {
            return x * x + y * y + z * z + w * w;
        }

        inline f32 GetMagnitude() const
        {
            return sqrt(GetSqrMagnitude());
        }

        inline TVector4 GetNormalized() const
        {
            return *this / GetMagnitude();
        }

        inline static f32 Dot(TVector4 a, TVector4 b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        inline f32 Dot(TVector4 b) const
        {
            return x * b.x + y * b.y + z * b.z + w * b.w;
        }

        inline static TVector4 Cross(TVector4 a, TVector4 b)
        {
            return TVector4(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x, a.w * b.w);
        }

        inline TVector4 Cross(TVector4 b) const
        {
            return TVector4(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x, w * b.w);
        }

        /// Signed angle in radians between 2 vectors
        inline static f32 SignedAngle(TVector4 a, TVector4 b)
        {
            return Math::ACos(Dot(a, b) / (a.GetMagnitude() * b.GetMagnitude())); // TODO
        }

        /// Signed angle in radians between 2 vectors
        inline f32 SignedAngle(TVector4 b) const
        {
            return SignedAngle(*this, b);
        }

        inline static TVector4<f32> Lerp(TVector4 from, TVector4 to, f32 t)
        {
            return TVector4<f32>(Math::Lerp(from.x, to.x, t), Math::Lerp(from.y, to.y, t), Math::Lerp(from.z, to.z, t), Math::Lerp(from.w, to.w, t));
        }

        inline static TVector4<f32> LerpUnclamped(TVector4 from, TVector4 to, f32 t)
        {
            return TVector4<f32>(Math::LerpUnclamped(from.x, to.x, t), Math::LerpUnclamped(from.y, to.y, t), Math::LerpUnclamped(from.z, to.z, t), Math::LerpUnclamped(from.w, to.w, t));
        }

        inline String ToString() const
        {
            return String::Format("({}, {}, {}, {})", x, y, z, w);
        }
    };

    class CORE_API Rect
    {
    public:

        Rect() : left(0), top(0), right(0), bottom(0)
        {}

        Rect(const Vec4& vec) : left(vec.left), top(vec.top), right(vec.right), bottom(vec.bottom)
        {}

        Rect(const Vec2& min, const Vec2& max) : min(min), max(max)
        {}

        Rect(f32 left, f32 top, f32 right, f32 bottom) : left(left), top(top), right(right), bottom(bottom)
        {}

        static Rect FromSize(const Vec2& min, const Vec2& size)
        {
            return Rect(min, min + size);
        }

        static Rect FromSize(f32 x, f32 y, f32 w, f32 h)
        {
            return Rect(x, y, x + w, y + h);
        }

        union {
            struct {
                f32 left, top, right, bottom;
            };
            struct {
                Vec2 min, max;
            };
        };

        inline Vec2 GetSize() const
        {
            return max - min;
        }

        inline Vec4 ToVec4() const
        {
            return Vec4(left, top, right, bottom);
        }

        inline bool operator==(const Rect& rhs) const
        {
            return (left == rhs.left) && (top == rhs.top) && (right == rhs.right) && (bottom == rhs.bottom);
        }

        inline bool operator!=(const Rect& rhs) const
        {
            return !(*this == rhs);
        }

        inline Rect operator+(const Rect& rhs)
        {
            return Rect(left + rhs.left, top + rhs.top, right + rhs.right, bottom + rhs.bottom);
        }

        inline Rect operator-(const Rect& rhs)
        {
            return Rect(left - rhs.left, top - rhs.top, right - rhs.right, bottom - rhs.bottom);
        }

        inline Rect& operator+=(const Rect& rhs)
        {
            left += rhs.left; top += rhs.top; right += rhs.right; bottom += rhs.bottom;
            return *this;
        }

        inline Rect& operator-=(const Rect& rhs)
        {
            left -= rhs.left; top -= rhs.top; right -= rhs.right; bottom -= rhs.bottom;
            return *this;
        }

        inline SIZE_T GetHash() const
        {
            SIZE_T hash = CE::GetHash(left);
            CombineHash(hash, top);
            CombineHash(hash, right);
            CombineHash(hash, bottom);
            return hash;
        }
        
    };

    template<typename T>
    TVector2<T>::TVector2(TVector3<T> vec3) : x(vec3.x), y(vec3.y)
    {}

    template<typename T>
    TVector2<T>::TVector2(TVector4<T> vec4) : x(vec4.x), y(vec4.y)
    {}

    template<typename T>
    TVector3<T>::TVector3(TVector4<T> vec4) : x(vec4.x), y(vec4.y), z(vec4.z)
    {
        xyz[3] = 0;
    }

    template<typename T>
    TVector3<T>::TVector3(TVector2<T> vec2) : x(vec2.x), y(vec2.y), z(0)
    {
        xyz[3] = 0;
    }

    template<typename T>
    TVector4<T>::TVector4(TVector2<T> vec2) : x(vec2.x), y(vec2.y), z(0), w(0)
    {}

	template<typename T>
	TVector4<T>::TVector4(TVector2<T> xy, TVector2<T> zw) : x(xy.x), y(xy.y), z(zw.x), w(zw.y)
	{}

    template<typename T>
    TVector4<T>::TVector4(TVector3<T> vec3) : x(vec3.x), y(vec3.y), z(vec3.z), w(0)
    {}

    template<typename T>
    inline TVector2<T> operator*(s32 lhs, TVector2<T> rhs)
    {
        return rhs * lhs;
    }

    template<typename T>
    inline TVector3<T> operator*(s32 lhs, TVector3<T> rhs)
    {
        return rhs * lhs;
    }
    
    template<typename T>
    inline TVector4<T> operator*(s32 lhs, TVector4<T> rhs)
    {
        return rhs * lhs;
    }

    template<typename T>
    inline TVector2<T> operator*(f32 lhs, TVector2<T> rhs)
    {
        return rhs * lhs;
    }

    template<typename T>
    inline TVector3<T> operator*(f32 lhs, TVector3<T> rhs)
    {
        return rhs * lhs;
    }

    template<typename T>
    inline TVector4<T> operator*(f32 lhs, TVector4<T> rhs)
    {
        return rhs * lhs;
    }

} // namespace CE

/// fmt user-defined Formatter for CE::Vec2
template <> struct fmt::formatter<CE::Vec2> {
    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // Return an iterator past the end of the parsed range:
        return ctx.end();
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext>
    auto format(const CE::Vec2& vec2, FormatContext& ctx) const -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "{}", vec2.ToString());
    }
};

/// fmt user-defined Formatter for CE::Vec3
template <> struct fmt::formatter<CE::Vec3> {
    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // Return an iterator past the end of the parsed range:
        return ctx.end();
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext>
    auto format(const CE::Vec3& vec3, FormatContext& ctx) const -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "{}", vec3.ToString());
    }
};

/// fmt user-defined Formatter for CE::Vec4
template <> struct fmt::formatter<CE::Vec4> {
    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // Return an iterator past the end of the parsed range:
        return ctx.end();
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext>
    auto format(const CE::Vec4& vec4, FormatContext& ctx) const -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "{}", vec4.ToString());
    }
};

/// fmt user-defined Formatter for CE::Vec2i
template <> struct fmt::formatter<CE::Vec2i> {
    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // Return an iterator past the end of the parsed range:
        return ctx.end();
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext>
    auto format(const CE::Vec2i& vec2, FormatContext& ctx) const -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "{}", vec2.ToString());
    }
};

/// fmt user-defined Formatter for CE::Vec3i
template <> struct fmt::formatter<CE::Vec3i> {
    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // Return an iterator past the end of the parsed range:
        return ctx.end();
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext>
    auto format(const CE::Vec3i& vec3, FormatContext& ctx) const -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "{}", vec3.ToString());
    }
};

/// fmt user-defined Formatter for CE::Vec4i
template <> struct fmt::formatter<CE::Vec4i> {
    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // Return an iterator past the end of the parsed range:
        return ctx.end();
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext>
    auto format(const CE::Vec4i& vec4, FormatContext& ctx) const -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "{}", vec4.ToString());
    }
};

