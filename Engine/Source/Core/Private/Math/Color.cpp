#include "CoreMinimal.h"

namespace CE
{
    static const Color hueValues[] = { Color(1, 0, 0), Color(1, 1, 0), Color(0, 1, 0),
    									Color(0, 1, 1), Color(0, 0, 1), Color(1, 0, 1),
    									Color(1, 0, 0)
    };

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

		if (h >= 360)
			h = 359.999f;

		f32 rgbRange = v * s;
		f32 maxRGB = v;
		f32 minRGB = v - rgbRange;
		f32 hPrime = h / 60.0;
		f32 x1 = fmod(hPrime, 1.0);
		f32 x2 = 1.0 - fmod(hPrime, 1.0);

		if ((hPrime >= 0) && (hPrime < 1))
		{
			rgb.r = maxRGB;
			rgb.g = (x1 * rgbRange) + minRGB;
			rgb.b = minRGB;
		}
		else if ((hPrime >= 1) && (hPrime < 2))
		{
			rgb.r = (x2 * rgbRange) + minRGB;
			rgb.g = maxRGB;
			rgb.b = minRGB;
		}
		else if ((hPrime >= 2) && (hPrime < 3))
		{
			rgb.r = minRGB;
			rgb.g = maxRGB;
			rgb.b = (x1 * rgbRange) + minRGB;
		}
		else if ((hPrime >= 3) && (hPrime < 4))
		{
			rgb.r = minRGB;
			rgb.g = (x2 * rgbRange) + minRGB;
			rgb.b = maxRGB;
		}
		else if ((hPrime >= 4) && (hPrime < 5))
		{
			rgb.r = (x1 * rgbRange) + minRGB;
			rgb.g = minRGB;
			rgb.b = maxRGB;
		}
		else if ((hPrime >= 5) && (hPrime < 6))
		{
			rgb.r = maxRGB;
			rgb.g = minRGB;
			rgb.b = (x2 * rgbRange) + minRGB;
		}

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

