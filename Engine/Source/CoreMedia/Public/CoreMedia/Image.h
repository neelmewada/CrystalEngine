#pragma once

namespace CE
{
	enum class CMImageFormat
	{
		Undefined = 0,
		// Grayscale
		R,
		// Gray + Alpha
		RG,
		RGB,
		RGBA
	};
	ENUM_CLASS_FLAGS(CMImageFormat);

	enum class CMImageSourceFormat
	{
		None = 0,
		PNG,
		HDR, EXR,
		BC1,
		BC3,
		BC4,
		BC5,
		BC6H,
		BC7,
	};

    struct CMImageInfo
    {
    public:
        u32 x = 0;
		u32 y = 0;
		u32 numChannels = 0;
		CMImageFormat format = CMImageFormat::Undefined;
		CMImageSourceFormat sourceFormat = CMImageSourceFormat::None;
		u32 bitDepth = 0;
		u32 bitsPerPixel = 0;

        const char* failureReason = nullptr;

        virtual bool IsValid() const { return x > 0 && y > 0 && bitDepth > 0 && bitsPerPixel > 0 && numChannels > 0 && numChannels <= 4 && 
			format != CMImageFormat::Undefined && sourceFormat != CMImageSourceFormat::None; }
    };

	/*
	* CMImage is a low-level image that can be used by RHI, Engine, Editor, etc.
	* It abstracts away all the image loading mechanisms to provide a 'raw' image to be used across the engine.
	* Currently, only PNG file format is supported.
	*/
    class COREMEDIA_API CMImage : private CMImageInfo
    {
    public:
        CMImage();
        virtual ~CMImage();

        // - Static API -

        static CMImageInfo GetImageInfoFromFile(const IO::Path& filePath);
        static CMImageInfo GetImageInfoFromMemory(unsigned char* buffer, int bufferLength);

		// Only supports PNG for now
        static CMImage LoadFromFile(IO::Path filePath);

		// Only supports PNG for now
        static CMImage LoadFromMemory(unsigned char* buffer, int bufferLength);

		/// Loads raw image from memory without allocating any memory
		static CMImage LoadRawImageFromMemory(unsigned char* buffer, CMImageFormat pixelFormat, u32 bitDepth, u32 bitsPerPixel);

		// - Encode API -

		// Encodes raw image pixel data to PNG format
		static bool EncodePNG(const CMImage& source, Stream* outStream, CMImageFormat pixelFormat, u32 bitDepth = 8);

		// Decodes raw image pixel data from PNG format
		static CMImage DecodePNG(const CMImage& source, MemoryStream* inStream);

#if PLATFORM_DESKTOP
		// Encodes raw image data to BCn format (BC7, BC1, etc)
		static bool EncodeToBCn(const CMImage& source, Stream* outStream, CMImageSourceFormat toBCnFormat, float quality = 0.25f);
#endif

        // - Public API -

        /// Always call the Free() function when image is no longer needed!
        void Free();

        inline unsigned char* GetDataPtr() const { return data; }
		inline u32 GetDataSize() const { return bitsPerPixel / 8 * x * y; }
        inline u32 GetWidth() const { return x; }
        inline u32 GetHeight() const { return y; }
        inline u32 GetNumChannels() const { return numChannels; }
		inline CMImageFormat GetFormat() const { return format; }
		inline CMImageSourceFormat GetSourceFormat() const { return sourceFormat; }
		inline u32 GetBitDepth() const { return bitDepth; }
		inline u32 GetBitsPerPixel() const { return bitsPerPixel; }

        bool IsValid() const override { return data != nullptr && CMImageInfo::IsValid(); }

        inline const char* GetFailureReason() const { return failureReason; }

    private:

		static CMImageInfo GetPNGImageInfo(MemoryStream* stream);
		static CMImage LoadPNGImage(MemoryStream* stream);

        unsigned char* data = nullptr;
		bool allocated = true;
    };

}
