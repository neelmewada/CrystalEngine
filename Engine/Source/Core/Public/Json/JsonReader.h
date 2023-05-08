#pragma once


namespace CE
{

    enum class JsonParseError
    {
        None = 0,
        InvalidString,
        InvalidNumber,
        InvalidKeyword,
        EmptyStack,
        OutOfBounds,
        InvalidObject,
        InvalidArray,
    };

    class CORE_API JsonReader
    {
    public:
        static JsonReader Create(Stream* stream)
        {
            return JsonReader(stream);
        }

        // - Public API -

        bool ParseNext(JsonReadInstruction& instruction);

        // - Internal API -

        // Internal use only
        bool ParseNextInObject(JsonReadInstruction& instruction);

        // Internal use only
        bool ParseNextInArray(JsonReadInstruction& instruction);

        // Internal use only
        bool ParseNextToken(JsonToken& outToken, String& outLexeme);
        
        // Internal use only
        bool NextAvailable();

        // - Getters & Setters -

        const String& GetErrorMessage() const
        {
            return errorMessage;
        }

        JsonParseError GetError() const
        {
            return parseError;
        }

        const String& GetIdentifier() const
        {
            return identifier;
        }

        const String& GetStringValue() const
        {
            return stringValue;
        }

        f64 GetNumberValue() const
        {
            return numberValue;
        }

        bool GetBoolValue() const
        {
            return boolValue;
        }

    protected:

        JsonReader()
        {}

        JsonReader(Stream* inStream)
            : stream(inStream)
        {}

        // Read variables
        String identifier = "";
        String stringValue = "";
        f64 numberValue = 0;
        bool boolValue = false;

        // State variables
        
        JsonToken currentToken = JsonToken::None;
        JsonToken previousToken = JsonToken::None;
        bool insideString = false;
        
        Stream* stream = nullptr;
        JsonParseError parseError = JsonParseError::None;
        String errorMessage = "";
        
        Array<JsonValueType> parserStack{};
    };
    
} // namespace CE
