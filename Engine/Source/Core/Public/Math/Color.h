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

        static Color FromRGBA32(u8 r, u8 g, u8 b, u8 a = (u8)255)
        {
            return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        }

        static Color FromRGBHex(u32 hex)
        {
            return FromRGBA32((u8)(hex >> 16), (u8)(hex >> 8), (u8)(hex));
        }

        static Color FromRGBAHex(u32 hex)
        {
            return FromRGBA32((u8)(hex >> 24), (u8)(hex >> 16), (u8)(hex >> 8), (u8)(hex));
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

        inline f32 operator[](u32 index)
        {
            if (index == 0)
            {
                return r;
            }
            else if (index == 1)
            {
                return g;
            }
            else if (index == 2)
            {
                return b;
            }
            else if (index == 3)
            {
                return a;
            }
            return 0;
        }

        // - Preset Colors -

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

        inline static Color Black()
        {
            return Color(0, 0, 0, 1);
        }

        inline static Color Yellow()
        {
            return Color(1, 1, 0, 1);
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
