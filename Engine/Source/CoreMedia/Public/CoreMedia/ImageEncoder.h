#pragma once

namespace CE
{
    
    class COREMEDIA_API CMImageEncoder final
    {
    public:

        enum Quality
        {
            Quality_Preview = 0,
            Quality_Fast,
            Quality_Normal,
            Quality_Slow
        };

        CMImageEncoder();
        ~CMImageEncoder();

        inline const String& GetErrorMessage() const { return errorMessage; }

        bool EncodeToBCn(const CMImage& image, BinaryBlob& outData, CMImageSourceFormat destBCnFormat, Quality quality = Quality_Normal);

    private:

        String errorMessage = "";

    };

} // namespace CE
