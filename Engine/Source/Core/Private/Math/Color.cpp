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

        int i = static_cast<int>(h / 60) % 6;
        f32 f = (h / 60) - i;
        f32 p = v * (1 - s);
        f32 q = v * (1 - f * s);
        f32 t = v * (1 - (1 - f) * s);

        f32& r = rgb.r;
        f32& g = rgb.g;
        f32& b = rgb.b;

        switch (i) {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        case 5:
            r = v;
            g = p;
            b = q;
            break;
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

