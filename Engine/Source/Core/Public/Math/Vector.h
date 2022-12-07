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

    template<typename T>
    class TVector2
    {
    public:

        TVector2() : X(0), Y(0)
        {}

        TVector2(T x, T y) : X(x), Y(y)
        {}

        TVector2(TVector3<T> vec3);
        TVector2(TVector4<T> vec4);

        union {
            struct {
                T X, Y;
            };
            T XY[2];
        };

        inline TVector2 operator+(const TVector2& rhs) const
        {
            return TVector2(X + rhs.X, Y + rhs.Y);
        }

        inline TVector2 operator-(const TVector2& rhs) const
        {
            return TVector2(X - rhs.X, Y - rhs.Y);
        }

        inline TVector2 operator+() const
        {
            return *this;
        }

        inline TVector2 operator-() const
        {
            return TVector2(-X, -Y);
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
            return X == rhs.X && Y == rhs.Y;
        }

        inline bool operator!=(const TVector2& rhs) const
        {
            return X != rhs.X || Y != rhs.Y;
        }

        inline TVector2 operator*(s32 value) const
        {
            return TVector2(value * X, value * Y);
        }

        inline TVector2 operator*(u32 value) const
        {
            return TVector2(value * X, value * Y);
        }

        inline TVector2 operator*(f32 value) const
        {
            return TVector2(value * X, value * Y);
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

        inline TVector2 operator/(f32 value)
        {
            return TVector2(X / value, Y / value);
        }

        inline TVector2 operator/=(f32 value)
        {
            *this = *this / value;
            return *this;
        }

        inline f32 GetSqrMagnitude() const
        {
            return X * X + Y * Y;
        }

        inline f32 GetMagnitude() const
        {
            return sqrt(GetSqrMagnitude());
        }

        inline TVector2 GetNormalized()
        {
            return *this / GetMagnitude();
        }

        inline static T Dot(TVector2 a, TVector2 b)
        {
            return a.X * b.X + a.Y * b.Y;
        }

        inline f32 Dot(TVector2 b) const
        {
            return X * b.X + Y * b.Y;
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
            return TVector2<f32>(Math::Lerp(from.X, to.X, t), Math::Lerp(from.Y, to.Y, t));
        }

        inline static TVector2<f32> LerpUnclamped(TVector2 from, TVector2 to, f32 t)
        {
            return TVector2<f32>(Math::LerpUnclamped(from.X, to.X, t), Math::LerpUnclamped(from.Y, to.Y, t));
        }

        inline String ToString() const
        {
            return String::Format("({}, {})", X, Y);
        }
    };
    
    template<typename T>
    class TVector3
    {
    public:

        TVector3() : X(0), Y(0), Z(0)
        {}

        TVector3(T x, T y) : X(x), Y(y), Z(0)
        {}

        TVector3(T x, T y, T z) : X(x), Y(y), Z(z)
        {}

        TVector3(TVector2<T> vec2);
        TVector3(TVector4<T> vec4);

        union {
            struct {
                T X, Y, Z;
            };
            T XYZ[4]; // size/alignment is same as Vector4
        };

        inline T operator[](int index) const
        {
            return XYZ[index];
        }

        inline TVector3 operator+(const TVector3& rhs) const
        {
            return TVector3(X + rhs.X, Y + rhs.Y, Z + rhs.Z);
        }

        inline TVector3 operator-(const TVector3& rhs) const
        {
            return TVector3(X - rhs.X, Y - rhs.Y, Z - rhs.Z);
        }

        inline TVector3 operator+() const
        {
            return *this;
        }

        inline TVector3 operator-() const
        {
            return TVector3(-X, -Y, -Z);
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
            return X == rhs.X && Y == rhs.Y && Z == rhs.Z;
        }

        inline bool operator!=(const TVector3& rhs) const
        {
            return X != rhs.X || Y != rhs.Y || Z != rhs.Z;
        }

        inline TVector3 operator*(s32 value) const
        {
            return TVector3(value * X, value * Y, value * Z);
        }

        inline TVector3 operator*(u32 value) const
        {
            return TVector3(value * X, value * Y, value * Z);
        }

        inline TVector3 operator*(f32 value) const
        {
            return TVector3(value * X, value * Y, value * Z);
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

        inline TVector3 operator/(f32 value)
        {
            return TVector3(X / value, Y / value, Z / value);
        }

        inline TVector3 operator/=(f32 value)
        {
            *this = *this / value;
            return *this;
        }

        inline f32 GetSqrMagnitude() const
        {
            return X * X + Y * Y + Z * Z;
        }

        inline f32 GetMagnitude() const
        {
            return sqrt(GetSqrMagnitude());
        }

        inline TVector3 GetNormalized()
        {
            return *this / GetMagnitude();
        }

        inline static T Dot(TVector3 a, TVector3 b)
        {
            return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
        }

        inline f32 Dot(TVector3 b) const
        {
            return X * b.X + Y * b.Y + Z * b.Z;
        }

        inline static TVector3 Cross(TVector3 a, TVector3 b)
        {
            return TVector3(a.Y * b.Z - a.Z * b.Y, a.Z * b.X - a.X * b.Z, a.X * b.Y - a.Y * b.X);
        }

        inline TVector3 Cross(TVector3 b) const
        {
            return TVector3(Y * b.Z - Z * b.Y, Z * b.X - X * b.Z, X * b.Y - Y * b.X);
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
            return TVector3<f32>(Math::Lerp(from.X, to.X, t), Math::Lerp(from.Y, to.Y, t), Math::Lerp(from.Z, to.Z, t));
        }

        inline static TVector3<f32> LerpUnclamped(TVector3 from, TVector3 to, f32 t)
        {
            return TVector3<f32>(Math::LerpUnclamped(from.X, to.X, t), Math::LerpUnclamped(from.Y, to.Y, t), Math::LerpUnclamped(from.Z, to.Z, t));
        }

        inline String ToString() const
        {
            return String::Format("({}, {}, {})", X, Y, Z);
        }
    };

    template<typename T>
    class TVector4
    {
    public:

        TVector4() : X(0), Y(0), Z(0), W(0)
        {}

        TVector4(T x, T y) : X(x), Y(y), Z(0), W(0)
        {}

        TVector4(T x, T y, T z) : X(x), Y(y), Z(z), W(0)
        {}

        TVector4(T x, T y, T z, T w) : X(x), Y(y), Z(z), W(w)
        {}

        TVector4(TVector2<T> vec2);
        TVector4(TVector3<T> vec3);

        union {
            struct {
                T X, Y, Z, W;
            };
            T XYZW[4];
        };

        inline T& operator[](int index)
        {
            return XYZW[index];
        }

        inline const T& operator[](int index) const
        {
            return XYZW[index];
        }

        inline TVector4 operator+(const TVector4& rhs) const
        {
            return TVector4(X + rhs.X, Y + rhs.Y, Z + rhs.Z, W + rhs.W);
        }

        inline TVector4 operator-(const TVector4& rhs) const
        {
            return TVector4(X - rhs.X, Y - rhs.Y, Z - rhs.Z, W - rhs.W);
        }

        inline TVector4 operator+() const
        {
            return *this;
        }

        inline TVector4 operator-() const
        {
            return TVector4(-X, -Y, -Z, -W);
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
            return X == rhs.X && Y == rhs.Y && Z == rhs.Z && W == rhs.W;
        }

        inline bool operator!=(const TVector4& rhs) const
        {
            return X != rhs.X || Y != rhs.Y || Z != rhs.Z || W != rhs.W;
        }

        inline TVector4 operator*(s32 value) const
        {
            return TVector4(value * X, value * Y, value * Z, value * W);
        }

        inline TVector4 operator*(u32 value) const
        {
            return TVector4(value * X, value * Y, value * Z, value * W);
        }

        inline TVector4 operator*(f32 value) const
        {
            return TVector4(value * X, value * Y, value * Z, value * W);
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

        inline TVector4 operator/(f32 value)
        {
            return TVector4(X / value, Y / value, Z / value, W / value);
        }

        inline TVector4 operator/=(f32 value)
        {
            *this = *this / value;
            return *this;
        }

        inline f32 GetSqrMagnitude() const
        {
            return X * X + Y * Y + Z * Z + W * W;
        }

        inline f32 GetMagnitude() const
        {
            return sqrt(GetSqrMagnitude());
        }

        inline TVector4 GetNormalized()
        {
            return *this / GetMagnitude();
        }

        inline static f32 Dot(TVector4 a, TVector4 b)
        {
            return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
        }

        inline f32 Dot(TVector4 b) const
        {
            return X * b.X + Y * b.Y + Z * b.Z + W * b.W;
        }

        inline static TVector4 Cross(TVector4 a, TVector4 b)
        {
            return TVector4(a.Y * b.Z - a.Z * b.Y, a.Z * b.X - a.X * b.Z, a.X * b.Y - a.Y * b.X, a.W * b.W);
        }

        inline TVector4 Cross(TVector4 b) const
        {
            return TVector4(Y * b.Z - Z * b.Y, Z * b.X - X * b.Z, X * b.Y - Y * b.X, W * b.W);
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
            return TVector4<f32>(Math::Lerp(from.X, to.X, t), Math::Lerp(from.Y, to.Y, t), Math::Lerp(from.Z, to.Z, t), Math::Lerp(from.W, to.W, t));
        }

        inline static TVector4<f32> LerpUnclamped(TVector4 from, TVector4 to, f32 t)
        {
            return TVector4<f32>(Math::LerpUnclamped(from.X, to.X, t), Math::LerpUnclamped(from.Y, to.Y, t), Math::LerpUnclamped(from.Z, to.Z, t), Math::LerpUnclamped(from.W, to.W, t));
        }

        inline String ToString() const
        {
            return String::Format("({}, {}, {}, {})", X, Y, Z, W);
        }
    };

    template<typename T>
    TVector2<T>::TVector2(TVector3<T> vec3) : X(vec3.X), Y(vec3.Y)
    {}

    template<typename T>
    TVector2<T>::TVector2(TVector4<T> vec4) : X(vec4.X), Y(vec4.Y)
    {}

    template<typename T>
    TVector3<T>::TVector3(TVector4<T> vec4) : X(vec4.X), Y(vec4.Y), Z(vec4.Z)
    {
        XYZ[3] = 0;
    }

    template<typename T>
    TVector3<T>::TVector3(TVector2<T> vec2) : X(vec2.X), Y(vec2.Y), Z(0)
    {
        XYZ[3] = 0;
    }

    template<typename T>
    TVector4<T>::TVector4(TVector2<T> vec2) : X(vec2.X), Y(vec2.Y), Z(0), W(0)
    {}

    template<typename T>
    TVector4<T>::TVector4(TVector3<T> vec3) : X(vec3.X), Y(vec3.Y), Z(vec3.Z), W(0)
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

