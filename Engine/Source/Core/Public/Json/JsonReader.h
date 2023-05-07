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
    };

    class CORE_API JsonReader
    {
    public:
        static JsonReader Create(Stream* stream)
        {
            return JsonReader(stream);
        }

        bool ReadNext(JsonReadInstruction& instruction)
        {
            if (stream == nullptr)
            {
                return false;
            }
            
            return false;
        }

        const String& GetErrorMessage() const
        {
            return errorMessage;
        }

        JsonParseError GetError() const
        {
            return parseError;
        }

    public:

        bool ParseNextToken(JsonToken& outToken, String& outLexeme);

        bool NextAvailable();

        JsonReader()
        {}

        JsonReader(Stream* inStream)
            : stream(inStream)
        {}
        
        JsonToken currentToken = JsonToken::None;
        JsonToken previousToken = JsonToken::None;
        bool insideString = false;
        
        Stream* stream = nullptr;
        JsonParseError parseError = JsonParseError::None;
        String errorMessage = "";
        
        Array<JsonValueType> parserStack{};
    };
    
} // namespace CE
