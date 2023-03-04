#pragma once

namespace CE
{
    
    class CORE_API Color
    {
    public:
        Color() : r(0), g(0), b(0), a(0)
        {

        }

        Color(f32 r, f32 g, f32 b, f32 a)
            : r(r), g(g), b(b), a(a)
        {
            
        }

        Color(f32 r, f32 g, f32 b)
            : r(r), g(g), b(b), a(1)
        {

        }

        Color(Vec4 v)
            : r(v.x), g(v.y), b(v.z), a(v.w)
        {

        }

        Color(Vec3 v)
            : r(v.x), g(v.y), b(v.z), a(1)
        {

        }

        inline static Color Red()
        {
            return Color(1, 0, 0, 1);
        }

        inline static Color Green()
        {
            return Color(0, 1, 0, 1);
        }

        inline static Color Blue()
        {
            return Color(0, 0, 1, 1);
        }

        inline Color operator*(s32 value) const
        {
            return Color(value * r, value * g, value * b, value * a);
        }

        inline Color operator*(u32 value) const
        {
            return Color(value * r, value * g, value * b, value * a);
        }

        inline Color operator*(f32 value) const
        {
            return Color(value * r, value * g, value * b, value * a);
        }

        inline Color operator*=(s32 value)
        {
            *this = *this * value;
            return *this;
        }

        inline Color operator*=(u32 value)
        {
            *this = *this * value;
            return *this;
        }

        inline Color operator*=(f32 value)
        {
            *this = *this * value;
            return *this;
        }

        inline Color operator/(f32 value) const
        {
            return Color(r / value, g / value, b / value, a / value);
        }

        inline Color operator/=(f32 value)
        {
            *this = *this / value;
            return *this;
        }

    public:

        union {
            struct {
                f32 r, g, b, a;
            };
            f32 rgba[4] = { 0, 0, 0, 0 };
        };
    };

} // namespace CE
