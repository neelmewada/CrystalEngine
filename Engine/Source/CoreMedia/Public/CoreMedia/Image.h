#pragma once

namespace CE
{
    struct CMImageInfo
    {
    public:
        int x = 0;
        int y = 0;
        int numChannels = 0;

        const char* failureReason = nullptr;

        virtual bool IsValid() const { return x > 0 && y > 0 && numChannels > 0; }
    };

    class COREMEDIA_API CMImage : private CMImageInfo
    {
    private:
        CMImage();
    public:
        virtual ~CMImage();

        // - Static API -

        static CMImageInfo GetImageInfoFromFile(IO::Path filePath);
        static CMImageInfo GetImageInfoFromMemory(unsigned char* buffer, int bufferLength);

        static CMImage LoadFromFile(IO::Path filePath, int desiredNumChannels = 0);
        static CMImage LoadFromMemory(unsigned char* buffer, int bufferLength, int desiredNumChannels = 0);

        // - Public API -

        /// Always call the Free() function when image is no longer needed!
        void Free();

        inline int GetWidth() const { return x; }
        inline int GetHeight() const { return y; }
        inline int GetNumChannels() const { return numChannels; }

        virtual bool IsValid() const override { return data != nullptr && CMImageInfo::IsValid(); }

        inline const char* GetFailureReason() const { return failureReason; }

    private:
        unsigned char* data = nullptr;
    };

}
