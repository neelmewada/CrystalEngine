#pragma once

namespace CE
{
    class JsonValue;
    class StructuredStream;

    class CORE_API JsonStreamInputFormatter : public StructuredStreamFormatter
    {
    public:
        JsonStreamInputFormatter(Stream& stream);
        ~JsonStreamInputFormatter();
        
        Stream& GetUnderlyingStream() override;
        bool CanRead() override;
        bool CanWrite() override;

        bool IsValid() override;

        bool IsRoot() override;
        
    private:
        Stream& stream;
        JsonValue* internalValue = nullptr;
        //StructuredStreamEntry& currentEntry;
    };

    
}
