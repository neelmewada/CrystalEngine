#pragma once

#include "Math/Math.h"
#include "Math/Vector.h"

namespace CE
{
    
    class CORE_API Matrix4x4
    {
    public:

        Matrix4x4()
        {
            memset(Rows, 0, sizeof(Rows));
        }

        Matrix4x4(Vec4 rows[4])
        {
            memcpy(Rows, rows, sizeof(Rows));
        }

        Matrix4x4(std::initializer_list<Vec4> rows)
        {
            memcpy(Rows, rows.begin(), sizeof(Rows));
        }

        Matrix4x4(const Matrix4x4& copy)
        {
            memcpy(Rows, copy.Rows, sizeof(Rows));
        }

        CE_INLINE static Matrix4x4 Zero()
        {
            Vec4 rows[4] = {
                Vec4{ 0, 0, 0, 0 },
                Vec4{ 0, 0, 0, 0 },
                Vec4{ 0, 0, 0, 0 },
                Vec4{ 0, 0, 0, 0 }
            };

            return Matrix4x4(rows);
        }

        CE_INLINE static Matrix4x4 Identity()
        {
            Vec4 rows[4] = {
                Vec4{ 1, 0, 0, 0 },
                Vec4{ 0, 1, 0, 0 },
                Vec4{ 0, 0, 1, 0 },
                Vec4{ 0, 0, 0, 1 }
            };

            return Matrix4x4(rows);
        }

        CE_INLINE Vec4& operator[](int index)
        {
            return Rows[index];
        }

        CE_INLINE const Vec4& operator[](int index) const
        {
            return Rows[index];
        }

        Matrix4x4 operator+(const Matrix4x4& rhs) const;
        Matrix4x4 operator-(const Matrix4x4& rhs) const;

        CE_INLINE Matrix4x4 operator+=(const Matrix4x4& rhs)
        {
            *this = *this + rhs;
            return *this;
        }

        CE_INLINE Matrix4x4 operator-=(const Matrix4x4& rhs)
        {
            *this = *this - rhs;
            return *this;
        }

        Matrix4x4 operator*(f32 rhs) const;
        Matrix4x4 operator/(f32 rhs) const;

        CE_INLINE Matrix4x4 operator*=(f32 rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        CE_INLINE Matrix4x4 operator/=(f32 rhs)
        {
            *this = *this / rhs;
            return *this;
        }

        CE_INLINE Matrix4x4 operator*(const Matrix4x4& rhs) const
        {
            return Multiply(*this, rhs);
        }

        CE_INLINE Matrix4x4 operator*=(const Matrix4x4& rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        CE_INLINE bool operator==(const Matrix4x4& rhs) const
        {
            return Rows[0] == rhs.Rows[0] && Rows[1] == rhs.Rows[1] && Rows[2] == rhs.Rows[2] && Rows[3] == rhs.Rows[3];
        }

        CE_INLINE bool operator!=(const Matrix4x4& rhs) const
        {
            return !(*this == rhs);
        }

        static Matrix4x4 Multiply(const Matrix4x4& lhs, const Matrix4x4& rhs);

        static Matrix4x4 GetTranspose(const Matrix4x4& mat);

        CE_INLINE Matrix4x4 GetTranspose() const
        {
            return GetTranspose(*this);
        }

        CE_INLINE void Transpose()
        {
            *this = GetTranspose(*this);
        }

        CE_INLINE Matrix4x4 GetInverse() const
        {
            return GetInverse(*this);
        }

        CE_INLINE void Invert()
        {
            *this = GetInverse(*this);
        }

        CE_INLINE String ToString() const
        {
            return String::Format("[{} {} {} {}]\n[{} {} {} {}]\n[{} {} {} {}]\n[{} {} {} {}]",
                Rows[0][0], Rows[0][1], Rows[0][2], Rows[0][3],
                Rows[1][0], Rows[1][1], Rows[1][2], Rows[1][3],
                Rows[2][0], Rows[2][1], Rows[2][2], Rows[2][3],
                Rows[3][0], Rows[3][1], Rows[3][2], Rows[3][3]);
        }

    private:
        /// Function to get determinant of mat[p][q]
        static void GetCofactor(const Matrix4x4& mat, Matrix4x4& cofactor, s32 p, s32 q, s32 n);

        static int GetDeterminant(const Matrix4x4& mat, s32 n);

        static Matrix4x4 GetAdjoint(const Matrix4x4& mat);

        static Matrix4x4 GetInverse(const Matrix4x4& mat);
        
    public:

        Vec4 Rows[4];
    };

} // namespace CE

CE_RTTI_POD(CE, Matrix4x4)

/// fmt user-defined Formatter for CE::Matrix4x4
template <> struct fmt::formatter<CE::Matrix4x4> {
    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // Return an iterator past the end of the parsed range:
        return ctx.end();
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext>
    auto format(const CE::Matrix4x4& mat, FormatContext& ctx) const -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "{}", mat.ToString());
    }
};
