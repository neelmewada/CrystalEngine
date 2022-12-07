#pragma once

#include "Math/Math.h"
#include "Math/Vector.h"

namespace CE
{
    
    class CORE_API Matrix
    {
    public:

        Matrix()
        {
            memset(Rows, 0, sizeof(Rows));
        }

        Matrix(Vec4 rows[4])
        {
            memcpy(Rows, rows, sizeof(Rows));
        }

        Matrix(std::initializer_list<Vec4> rows)
        {
            memcpy(Rows, rows.begin(), sizeof(Rows));
        }

        Matrix(const Matrix& copy)
        {
            memcpy(Rows, copy.Rows, sizeof(Rows));
        }

        inline static Matrix Zero()
        {
            Vec4 rows[4] = {
                Vec4{ 0, 0, 0, 0 },
                Vec4{ 0, 0, 0, 0 },
                Vec4{ 0, 0, 0, 0 },
                Vec4{ 0, 0, 0, 0 }
            };

            return Matrix(rows);
        }

        inline static Matrix Identity()
        {
            Vec4 rows[4] = {
                Vec4{ 1, 0, 0, 0 },
                Vec4{ 0, 1, 0, 0 },
                Vec4{ 0, 0, 1, 0 },
                Vec4{ 0, 0, 0, 1 }
            };

            return Matrix(rows);
        }

        inline Vec4& operator[](int index)
        {
            return Rows[index];
        }

        inline const Vec4& operator[](int index) const
        {
            return Rows[index];
        }

        Matrix operator+(const Matrix& rhs) const;
        Matrix operator-(const Matrix& rhs) const;

        inline Matrix operator+=(const Matrix& rhs)
        {
            *this = *this + rhs;
            return *this;
        }

        inline Matrix operator-=(const Matrix& rhs)
        {
            *this = *this - rhs;
            return *this;
        }

        Matrix operator*(f32 rhs) const;
        Matrix operator/(f32 rhs) const;

        inline Matrix operator*=(f32 rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        inline Matrix operator/=(f32 rhs)
        {
            *this = *this / rhs;
            return *this;
        }

        inline Matrix operator*(const Matrix& rhs) const
        {
            return Multiply(*this, rhs);
        }

        inline Matrix operator*=(const Matrix& rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        inline bool operator==(const Matrix& rhs) const
        {
            return Rows[0] == rhs.Rows[0] && Rows[1] == rhs.Rows[1] && Rows[2] == rhs.Rows[2] && Rows[3] == rhs.Rows[3];
        }

        inline bool operator!=(const Matrix& rhs) const
        {
            return !(*this == rhs);
        }

        static Matrix Multiply(const Matrix& lhs, const Matrix& rhs);

        static Matrix GetTranspose(const Matrix& mat);

        inline Matrix GetTranspose() const
        {
            return GetTranspose(*this);
        }

        inline void Transpose()
        {
            *this = GetTranspose(*this);
        }

        inline Matrix GetInverse() const
        {
            return GetInverse(*this);
        }

        inline void Invert()
        {
            *this = GetInverse(*this);
        }

        inline String ToString() const
        {
            return String::Format("[{} {} {} {}]\n[{} {} {} {}]\n[{} {} {} {}]\n[{} {} {} {}]",
                Rows[0][0], Rows[0][1], Rows[0][2], Rows[0][3],
                Rows[1][0], Rows[1][1], Rows[1][2], Rows[1][3],
                Rows[2][0], Rows[2][1], Rows[2][2], Rows[2][3],
                Rows[3][0], Rows[3][1], Rows[3][2], Rows[3][3]);
        }

    private:
        /// Function to get determinant of mat[p][q]
        static void GetCofactor(const Matrix& mat, Matrix& cofactor, s32 p, s32 q, s32 n);

        static int GetDeterminant(const Matrix& mat, s32 n);

        static Matrix GetAdjoint(const Matrix& mat);

        static Matrix GetInverse(const Matrix& mat);
        
    public:

        Vec4 Rows[4];
    };

} // namespace CE


/// fmt user-defined Formatter for CE::Matrix
template <> struct fmt::formatter<CE::Matrix> {
    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // Return an iterator past the end of the parsed range:
        return ctx.end();
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext>
    auto format(const CE::Matrix& mat, FormatContext& ctx) const -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "{}", mat.ToString());
    }
};
