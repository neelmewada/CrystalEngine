#pragma once

namespace CE
{

	ENUM()
	enum class TextureFormat
	{
		None = 0,
		RGBA32,
		BGRA32,
		ARGB32,
		RGB24,
		ARGB4444,

		RGB565,

		R8,
		// Single channel 16-bit integer
		R16,
		RG16,
		RG32,

		// Single channel 16-bit float
		RHalf,
		RGHalf,
		RGBAHalf,

		// Single channel 32-bit float
		RFloat,
		RGFloat,
		RGBAFloat,

		// Compressed formats
		BC4,
		BC6H,
		BC7_RGBA,
	};
	ENUM_CLASS_FLAGS(TextureFormat);

	ENUM()
	enum class TextureCompression
	{
		None = 0,
		BC7
	};
	ENUM_CLASS_FLAGS(TextureCompression);

	ENUM()
	enum class TextureSourceFormat
	{
		Unsupported = 0,
		PNG,
		HDR, EXR,
		JPG,
		BC4,
		BC6H,
		BC7,
	};
	ENUM_CLASS_FLAGS(TextureSourceFormat);

	ENUM()
	enum class TextureType : int
	{
		ECONST(Display = "Default (Color sRGB)")
		Default,
		NormalMap,
		Grayscale,
		HDR,
	};
	ENUM_CLASS_FLAGS(TextureType);

	ENUM()
	enum class TextureFilter : int
	{
		Linear = 0,
		Nearest
	};
	ENUM_CLASS_FLAGS(TextureFilter);
    
} // namespace CE

#include "TextureEnums.rtti.h"
