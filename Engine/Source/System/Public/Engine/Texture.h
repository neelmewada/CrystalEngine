#pragma once

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
	namespace Editor { class TextureAssetImporter; }
#endif

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

	namespace Private
	{
		STRUCT()
		struct SYSTEM_API TextureSource
		{
			CE_STRUCT(TextureSource)
		public:

			TextureSource();

			bool IsValid();

		private:
			
			// Automatically called when struct is destroyed
			void Release();

#if PAL_TRAIT_BUILD_EDITOR
			friend class CE::Editor::TextureAssetImporter;
#endif

			FIELD()
			BinaryBlob rawData{};

			FIELD()
			TextureSourceFormat sourceFormat = TextureSourceFormat::PNG;

		};
	}

	CLASS(Abstract)
	class SYSTEM_API Texture : public Asset
	{
		CE_CLASS(Texture, Asset)
	public:

		Texture();
		virtual ~Texture();

	protected:

		FIELD(ReadOnly)
		Private::TextureSource source{};

		FIELD()
		TextureType type = TextureType::Default;

		FIELD()
		TextureFilter filter = TextureFilter::Linear;

		FIELD()
		u32 width = 0;

		FIELD()
		u32 height = 0;

		FIELD()
		u32 depth = 1;

	};
    
} // namespace CE

#include "Texture.rtti.h"
