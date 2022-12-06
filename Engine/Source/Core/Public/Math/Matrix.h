#pragma once

#include "Math/Math.h"
#include "Math/Vector.h"

namespace CE
{
    
    class CORE_API Matrix
    {
    public:

        Matrix()
        {}

        Matrix(Vec4 rows[4])
        {
            memcpy(Rows, rows, sizeof(rows));
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

        Matrix operator+(const Matrix& rhs);
        Matrix operator-(const Matrix& rhs);

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

        Matrix operator*(f32 rhs);
        Matrix operator/(f32 rhs);

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

        inline Matrix operator*(const Matrix& rhs)
        {
            return Multiply(*this, rhs);
        }

        inline Matrix operator*=(const Matrix& rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        static Matrix Multiply(const Matrix& lhs, const Matrix& rhs);

        static Matrix Transpose(const Matrix& mat);

        inline Matrix Transpose()
        {
            return Transpose(*this);
        }
        
        Vec4 Rows[4];
    };

} // namespace CE
