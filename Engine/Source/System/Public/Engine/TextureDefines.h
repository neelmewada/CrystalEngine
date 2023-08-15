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
		BC3,
		BC4,
		BC6H,
		BC7,
	};
	ENUM_CLASS_FLAGS(TextureFormat);

	/// Texture source data format
	ENUM()
	enum class TextureSourceCompressionFormat
	{
		None = 0,
		PNG,
		HDR, EXR,
		JPG,
		BC1,
		BC3,
		BC4,
		BC5,
		BC6H,
		BC7,
	};
	ENUM_CLASS_FLAGS(TextureSourceCompressionFormat);


	ENUM()
	enum class TextureCompressionSettings : int
	{
		ECONST(Display = "Default (Color BC1/3)")
		Default,
		ECONST(Display = "Normal Map (BC5)")
		NormalMap,
		ECONST(Display = "Grayscale (R8/16 or BC4)")
		Grayscale,
		ECONST(Display = "HDR (BC6H)")
		HDR,
		ECONST(Display = "HDR Uncompressed (RGBA16 Half)")
		HDRUncompressed,
		ECONST(Display = "BC7 RGBA")
		BC7,
	};
	ENUM_CLASS_FLAGS(TextureCompressionSettings);

	ENUM()
	enum class TextureFilter : int
	{
		Linear = 0,
		Nearest,
		Cubic
	};
	ENUM_CLASS_FLAGS(TextureFilter);

	ENUM()
	enum class TextureAddressMode : int
	{
		Wrap = 0,
		Clamp,
		Repeat,
	};
	ENUM_CLASS_FLAGS(TextureAddressMode);

	ENUM()
	enum class TextureColorSpace : int
	{
		None = 0,
		SRGB
	};
	ENUM_CLASS_FLAGS(TextureColorSpace);
    
} // namespace CE

#include "TextureDefines.rtti.h"
