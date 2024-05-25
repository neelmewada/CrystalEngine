#include "CoreMinimal.h"

namespace CE
{
	Color Color::RGBA8(u8 r, u8 g, u8 b, u8 a)
	{
		return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}

	Color Color::RGBHex(u32 hex)
	{
		return RGBA8((u8)(hex >> 16), (u8)(hex >> 8), (u8)(hex));
	}

	Color Color::RGBAHex(u32 hex)
	{
		return RGBA8((u8)(hex >> 24), (u8)(hex >> 16), (u8)(hex >> 8), (u8)(hex));
	}

	Color Color::HSV(f32 h, f32 s, f32 v)
	{
        Color rgb{};
        rgb.a = 1.0f;

        float c = v * s; // Chroma
        float x = c * (1 - (f32)std::fabs(fmod(h / 60.0, 2) - 1));
        float m = v - c;

        float r_prime, g_prime, b_prime;

        if (0 <= h && h < 60) {
            r_prime = c;
            g_prime = x;
            b_prime = 0;
        }
        else if (60 <= h && h < 120) {
            r_prime = x;
            g_prime = c;
            b_prime = 0;
        }
        else if (120 <= h && h < 180) {
            r_prime = 0;
            g_prime = c;
            b_prime = x;
        }
        else if (180 <= h && h < 240) {
            r_prime = 0;
            g_prime = x;
            b_prime = c;
        }
        else if (240 <= h && h < 300) {
            r_prime = x;
            g_prime = 0;
            b_prime = c;
        }
        else {
            r_prime = c;
            g_prime = 0;
            b_prime = x;
        }

        rgb.r = r_prime + m;
        rgb.g = g_prime + m;
        rgb.b = b_prime + m;

        return rgb;
	}

	u32 Color::ToU32() const
    {
		return ((u32)(r * 255)) | ((u32)(g * 255) << 8) | ((u32)(b * 255) << 16) | ((u32)(a * 255) << 24);
    }

    Vec4 Color::ToVec4() const
    {
        return Vec4(r, g, b, a);
    }

    Vec3 Color::ToHSV() const
    {
        f32 cmax = std::max(r, std::max(g, b)); // maximum of r, g, b 
        f32 cmin = std::min(r, std::min(g, b)); // minimum of r, g, b 
        f32 diff = cmax - cmin; // diff of cmax and cmin. 
        f32 h = -1, s = -1;

        // if cmax and cmax are equal then h = 0 
        if (cmax == cmin)
            h = 0;

        // if cmax equal r then compute h 
        else if (cmax == r)
            h = fmod(60 * ((g - b) / diff) + 360, 360);

        // if cmax equal g then compute h 
        else if (cmax == g)
            h = fmod(60 * ((b - r) / diff) + 120, 360);

        // if cmax equal b then compute h 
        else if (cmax == b)
            h = fmod(60 * ((r - g) / diff) + 240, 360);

        // if cmax equal zero 
        if (cmax == 0)
            s = 0;
        else
            s = (diff / cmax) * 100;

        f32 v = cmax * 100;

        return Vec3(h, s, v);
    }

} // namespace CE

