#pragma once

namespace CE
{
    class Stream;
    class StructuredStream;

    class CORE_API JsonStreamOutputFormatter : public StructuredStreamFormatter
    {
    public:
        JsonStreamOutputFormatter(Stream& stream);
        ~JsonStreamOutputFormatter();
        
        Stream& GetUnderlyingStream() override;
        bool CanRead() override;
        bool CanWrite() override;

        bool IsValid() override;

        bool IsRoot() override;

        void EnterMap(const String& identifier) override;
        void ExitMap() override;

    private:
        PrettyJsonWriter writer;
        
        Stream& stream;
    };

    
} // namespace CE
