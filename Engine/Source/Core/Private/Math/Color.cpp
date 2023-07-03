#include "CoreMinimal.h"

namespace CE
{
	Color Color::FromRGBA32(u8 r, u8 g, u8 b, u8 a)
	{
		return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}

	Color Color::FromRGBHex(u32 hex)
	{
		return FromRGBA32((u8)(hex >> 16), (u8)(hex >> 8), (u8)(hex));
	}

	Color Color::FromRGBAHex(u32 hex)
	{
		return FromRGBA32((u8)(hex >> 24), (u8)(hex >> 16), (u8)(hex >> 8), (u8)(hex));
	}

	u32 Color::ToU32() const
    {
		return ((u32)(r * 255)) | ((u32)(g * 255) << 8) | ((u32)(b * 255) << 16) | ((u32)(a * 255) << 24);
    }
    
} // namespace CE

