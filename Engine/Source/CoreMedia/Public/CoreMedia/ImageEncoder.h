#pragma once

namespace CE
{
    
    class COREMEDIA_API CMImageEncoder final
    {
    public:

        CMImageEncoder();
        ~CMImageEncoder();

        inline const String& GetErrorMessage() const { return errorMessage; }

        bool EncodeToBCn(const IO::Path& sourceFile, BinaryBlob& outData, CMImageSourceFormat destBCnFormat, float quality = 0.1f);

    private:

        String errorMessage = "";

    };

} // namespace CE
