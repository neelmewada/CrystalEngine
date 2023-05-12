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

        void EnterMap() override;
        void ExitMap() override;
        
        void EnterArray() override;
        void ExitArray() override;
        
        void EnterField(const String& identifier) override;
        void ExitField() override;
        
        void EnterStringValue(const String& value) override;
        void EnterNumberValue(f64 value) override;
        void EnterBoolValue(bool value) override;
        void EnterNullValue() override;

    private:
        PrettyJsonWriter writer;
        
        Stream& stream;
    };

    
} // namespace CE
