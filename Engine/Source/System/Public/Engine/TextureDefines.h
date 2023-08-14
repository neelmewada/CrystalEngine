#pragma once

namespace CE
{
	/// Texture pixel format
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

		// Compressed pixel formats
		BC1,
		BC4,
		BC6H,
		BC7,
	};
	ENUM_CLASS_FLAGS(TextureFormat);

	ENUM()
	enum class TextureSourceCompressionFormat
	{
		None = 0,
		PNG,
		HDR, EXR,
		JPG,
		BC1,
		BC4,
		BC6H,
		BC7,
	};
	ENUM_CLASS_FLAGS(TextureSourceCompressionFormat);


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
		Nearest,
		Cubic
	};
	ENUM_CLASS_FLAGS(TextureFilter);
    
} // namespace CE

#include "TextureDefines.rtti.h"
