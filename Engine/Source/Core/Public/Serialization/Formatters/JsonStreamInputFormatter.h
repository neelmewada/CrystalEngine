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
        Stream& stream;
        
        String currentIdentifier = "";
        int currentArrayIndex = 0;
        
        JsonValue* rootValue = nullptr;
        Array<JsonValue*> valueStack{};
    };

    
}
