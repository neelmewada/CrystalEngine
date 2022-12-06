#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <limits>

#define TO_RADIANS(Degrees) Degrees * (float)M_PI / 180.0f
#define TO_DEGREES(Radians) Radians * 180.0f / (float)M_PI

namespace CE
{

    class CORE_API Math
    {
    public:
        Math() = delete;
        ~Math() = delete;

        static constexpr f32 PI = M_PI;

        inline static f32 ToDegrees(f32 radians) { return TO_DEGREES(radians); }
        inline static f32 ToRadians(f32 degrees) { return TO_RADIANS(degrees); }

        inline static f32 Sin(f32 radians) { return sin(radians); }
        inline static f32 ASin(f32 sine) { return asin(sine); }

        inline static f32 Cos(f32 radians) { return cos(radians); }
        inline static f32 ACos(f32 cosine) { return acos(cosine); }

        inline static f32 Tan(f32 radians) { return tan(radians); }
        inline static f32 ATan(f32 tangent) { return atan(tangent); }

        inline static f32 Abs(f32 value) { return abs(value); }
        inline static f64 Abs(f64 value) { return abs(value); }

        inline static s32 Abs(s32 value) { return abs(value); }
        inline static s64 Abs(s64 value) { return abs(value); }

        inline static f32 Sqrt(f32 value) { return sqrt(value); }

        template<typename T>
        inline static T Min(std::initializer_list<T> list)
        {
            auto min = std::numeric_limits<T>::max();

            for (auto entry : list)
            {
                if (entry < min)
                    min = entry;
            }

            return min;
        }

        template<typename T>
        inline static T Max(std::initializer_list<T> list)
        {
            auto max = std::numeric_limits<T>::min();

            for (auto entry : list)
            {
                if (entry > max)
                    max = entry;
            }

            return max;
        }

        template<typename T>
        inline static T Min(T a, T b)
        {
            return a < b ? a : b;
        }

        template<typename T>
        inline static T Max(T a, T b)
        {
            return a > b ? a : b;
        }

        template<typename T>
        inline static T Clamp(T value, T min, T max) { return Min(Max(value, min), max); }

        template<typename T>
        inline static T Clamp01(T value) { return Clamp<T>(value, 0, 1); }

        inline static f32 Lerp(f32 from, f32 to, f32 t) { return from * (1 - Clamp01(t)) + to * Clamp01(t); }

        inline static f32 LerpUnclamped(f32 from, f32 to, f32 t) { return from * (1 - t) + to * (t); }
        
    private:

    };
    
} // namespace CE

