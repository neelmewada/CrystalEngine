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

        bool TryEnterMap() override;
        bool TryEnterArray() override;
        bool TryEnterField(const String& identifier = "") override;
        
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

        StructuredStreamEntry* GetRootEntry() override;

        StructuredStreamEntry* GetEntryAt(StructuredStreamPosition position) override;
        StructuredStreamEntry::Type GetNextEntryType() override;

    private:
        Stream& stream;
        
        String currentIdentifier = "";
        int currentArrayIndex = 0;
        
        JsonValue* rootJson = nullptr;
        Array<JsonValue*> valueStack{};
        Array<StructuredStreamPrivate::EntryKey> positionStack{};

        StructuredStreamEntry* rootEntry;
        StructuredStreamEntry* currentEntry;
    };

    
}
