
#include "CoreMinimal.h"

namespace CE
{
    bool JsonReader::ParseNext(JsonReadInstruction& instruction)
    {
        if (!NextAvailable())
            return false;

        auto curState = JsonValueType::None;

        if (parserStack.NonEmpty())
        {
            curState = parserStack.Top();
        }

        bool success = false;
        JsonToken token = JsonToken::None;
        String lexeme = "";

        switch (curState)
        {
        case JsonValueType::Array:
             success = ParseNextInArray(instruction);
            break;
        case JsonValueType::Object:
            success = ParseNextInObject(instruction);
            break;
        default:
            success = ParseNextToken(token, lexeme);
            if (token == JsonToken::CurlyOpen)
            {
                instruction = JsonReadInstruction::ObjectStart;
            }
            else if (token == JsonToken::SquareOpen)
            {
                instruction = JsonReadInstruction::ArrayStart;
            }
            else if (token == JsonToken::CurlyClose)
            {
                instruction = JsonReadInstruction::ObjectEnd;
            }
            else if (token == JsonToken::SquareClose)
            {
                instruction = JsonReadInstruction::ArrayEnd;
            }
            else
            {
                return false;
            }
            break;
        }
        
        return success;
    }

    bool JsonReader::ParseNextInObject(JsonReadInstruction& instruction)
    {
        JsonToken token = JsonToken::None;
        String lexeme = "";

        if (!ParseNextToken(token, lexeme))
        {
            return false;
        }

        if (token == JsonToken::Comma) // Skip comma because they are separators
        {
            token = JsonToken::None;
            lexeme = "";
            if (!ParseNextToken(token, lexeme))
            {
                return false;
            }
        }

        if (token == JsonToken::None)
        {
            parseError = JsonParseError::InvalidObject;
            errorMessage = "Invalid token in object";
            return false;
        }

        if (token == JsonToken::CurlyClose) // End Of Object
        {
            instruction = JsonReadInstruction::ObjectEnd;
            return true;
        }

        if (token != JsonToken::Identifier)
        {
            parseError = JsonParseError::InvalidObject;
            errorMessage = "Identifier not found in object";
            return false;
        }

        identifier = lexeme;

        token = JsonToken::None;
        lexeme = "";
        if (!ParseNextToken(token, lexeme))
        {
            return false;
        }

        if (token != JsonToken::Colon)
        {
            parseError = JsonParseError::InvalidObject;
            errorMessage = "Identifier not followed by a colon";
            return false;
        }

        token = JsonToken::None;
        lexeme = "";
        if (!ParseNextToken(token, lexeme))
        {
            return false;
        }

        if (token == JsonToken::CurlyOpen)
        {
            instruction = JsonReadInstruction::ObjectStart;
            return true;
        }
        if (token == JsonToken::SquareOpen)
        {
            instruction = JsonReadInstruction::ArrayStart;
            return true;
        }
        if (token == JsonToken::String)
        {
            stringValue = lexeme;
            instruction = JsonReadInstruction::String;
            return true;
        }
        if (token == JsonToken::Number)
        {
            if (!String::TryParse(lexeme, numberValue))
                return false;
            instruction = JsonReadInstruction::Number;
            return true;
        }
        if (token == JsonToken::True || token == JsonToken::False)
        {
            boolValue = token == JsonToken::True ? true : false;
            instruction = JsonReadInstruction::Boolean;
            return true;
        }
        if (token == JsonToken::Null)
        {
            instruction = JsonReadInstruction::Null;
            return true;
        }

        parseError = JsonParseError::InvalidObject;
        errorMessage = "Failed to parse an entry: " + identifier;
        return false;
    }

    bool JsonReader::ParseNextInArray(JsonReadInstruction& instruction)
    {
        JsonToken token = JsonToken::None;
        String lexeme = "";

        if (!ParseNextToken(token, lexeme))
        {
            return false;
        }

        if (token == JsonToken::Comma) // Skip comma because they are just separators
        {
            token = JsonToken::None;
            lexeme = "";
            if (!ParseNextToken(token, lexeme))
            {
                return false;
            }
        }

        if (token == JsonToken::None)
        {
            parseError = JsonParseError::InvalidArray;
            errorMessage = "Invalid token in array";
            return false;
        }
        
        if (token == JsonToken::SquareClose) // End Of Array
        {
            instruction = JsonReadInstruction::ArrayEnd;
            return true;
        }

        // Item Values
        if (token == JsonToken::CurlyOpen)
        {
            instruction = JsonReadInstruction::ObjectStart;
            return true;
        }
        if (token == JsonToken::SquareOpen)
        {
            instruction = JsonReadInstruction::ArrayStart;
            return true;
        }
        if (token == JsonToken::String)
        {
            stringValue = lexeme;
            instruction = JsonReadInstruction::String;
            return true;
        }
        if (token == JsonToken::Number)
        {
            if (!String::TryParse(lexeme, numberValue))
                return false;
            instruction = JsonReadInstruction::Number;
            return true;
        }
        if (token == JsonToken::True || token == JsonToken::False)
        {
            boolValue = token == JsonToken::True ? true : false;
            instruction = JsonReadInstruction::Boolean;
            return true;
        }
        if (token == JsonToken::Null)
        {
            instruction = JsonReadInstruction::Null;
            return true;
        }
        
        parseError = JsonParseError::InvalidArray;
        errorMessage = "Failed to parse an entry: " + identifier;
        return false;
    }

    bool JsonReader::ParseNextToken(JsonToken& outToken, String& outLexeme)
    {
        if (stream->IsOutOfBounds())
        {
            parseError = JsonParseError::OutOfBounds;
            errorMessage = "Input stream out of bounds";
            return false;
        }

        bool insideString = false;
        previousToken = currentToken;

        while (!stream->IsOutOfBounds())
        {
            CHAR cur = stream->Read();

            if (cur == '\0')
            {
                return true;
            }

            switch (cur)
            {
            case '{':
                outToken = JsonToken::CurlyOpen;
                parserStack.Push(JsonValueType::Object);
                currentToken = outToken;
                return true;
            case '}':
                if (!parserStack.IsEmpty() && parserStack.Top() == JsonValueType::Object)
                {
                    outToken = JsonToken::CurlyClose;
                    parserStack.Pop();
                    currentToken = outToken;
                    return true;
                }
                return false;
            case '[':
                outToken = JsonToken::SquareOpen;
                parserStack.Push(JsonValueType::Array);
                currentToken = outToken;
                return true;
            case ']':
                if (!parserStack.IsEmpty() && parserStack.Top() == JsonValueType::Array)
                {
                    outToken = JsonToken::SquareClose;
                    parserStack.Pop();
                    currentToken = outToken;
                    return true;
                }
                parseError = JsonParseError::EmptyStack;
                errorMessage = "SquareClose token found at wrong position";
                return false;
            case ':':
                outToken = JsonToken::Colon;
                currentToken = outToken;
                return true;
            case ',':
                outToken = JsonToken::Comma;
                currentToken = outToken;
                return true;
			case '/': // Possibly a comment
				if (!stream->IsOutOfBounds())
				{
					CHAR next = stream->Read();
					if (next == '/') // Definitely a comment
					{
						while (!stream->IsOutOfBounds()) // Skip thru entire comment
						{
							next = stream->Read();
							if (next == '\n') // Stop at new-line
								break;
						}
						
						// Now that comment is skipped, read next actual token
						return ParseNextToken(outToken, outLexeme);
					}
					else // Not a comment, invalid character?
					{
						parseError = JsonParseError::InvalidKeyword;
						errorMessage = String::Format("Invalid character {}", next);
						return false;
					}
				}
				parseError = JsonParseError::OutOfBounds;
				errorMessage = "Stream out of bounds";
				return false;
            case '\"': // String start token
                {
                    outLexeme.Clear();
                    auto prev = cur;
                    while ((cur = stream->Read()) != '\0' && !stream->IsOutOfBounds())
                    {
                        if (prev == '\\') // If previous char was an escape character
                        {
                            if (cur == 'n')
                                outLexeme.Append('\n');
                            else if (cur == '\\')
							{
								outLexeme.Append('\\');
								prev = cur = 0; // Do not set prev to '\' otherwise next character will be skipped
							}
                            else if (cur == 't')
                                outLexeme.Append('\t');
                            else if (cur == 'r')
                                outLexeme.Append('\r');
                            else if (cur == '\"') // Found an escaped string within this string entry
                                outLexeme.Append('\"');
                        }
                        else if (cur == '\"') // Found the end of string
                        {
                            break;
                        }
                        else if (cur != '\\') // cur should not be an escape character (back slash)
                        {
                            outLexeme.Append(cur);
                        }
                        prev = cur;
                    }
                    if (cur != '\"') // Could not find a valid end of the string
                    {
                        parseError = JsonParseError::InvalidString;
                        errorMessage = "Could not find end of string";
                        return false;
                    }
                    if (parserStack.IsEmpty())
                    {
                        parseError = JsonParseError::InvalidString;
                        errorMessage = "String is not part of a stack";
                        return false;
                    }
                    if ((parserStack.Top() == JsonValueType::Object && previousToken == JsonToken::Colon) ||
                        parserStack.Top() == JsonValueType::Array) // String value
                    {
                        outToken = JsonToken::String;
                        currentToken = outToken;
                        return true;
                    }
                    if (parserStack.Top() == JsonValueType::Object)
                    {
                        outToken = JsonToken::Identifier;
                        currentToken = outToken;
                        return true;
                    }
                    if (stream->IsOutOfBounds() || cur == '\0')
                    {
                        parseError = JsonParseError::OutOfBounds;
                    }
                    else
                    {
                        parseError = JsonParseError::InvalidString;
                    }
                    errorMessage = "Failed to parse string: " + outLexeme;
                    return false;
                }
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                {
                    outLexeme.Clear();
                    outLexeme.Append(cur);
                    while ((cur = stream->Read()) != '\0' && !stream->IsOutOfBounds())
                    {
                        if (!String::IsNumeric(cur) && cur != '.')
                        {
                            stream->Seek(-1, SeekMode::Current); // Go 1 step back
                            break;
                        }
                        outLexeme.Append(cur);
                    }
                    if ((parserStack.Top() == JsonValueType::Object && previousToken == JsonToken::Colon) ||
                        parserStack.Top() == JsonValueType::Array) // Number value
                    {
                        outToken = JsonToken::Number;
                        currentToken = outToken;
                        return true;
                    }
                    parseError = JsonParseError::InvalidNumber;
                    errorMessage = "Failed to parse number: " + outLexeme;
                    return false;
                }
                // Keywords:
            case 'f': // f-alse
            case 't': // t-rue
            case 'n': // n-ull
                {
                    auto start = cur;
                    
                    outLexeme.Clear();
                    while (cur != '\0' && !stream->IsOutOfBounds())
                    {
                        if (!String::IsAlphabet(cur))
                        {
                            stream->Seek(-1, SeekMode::Current); // Go 1 step back for next round
                            break;
                        }
                        outLexeme.Append(cur);
                        cur = stream->Read();
                    }

                    if (outLexeme == "false")
                    {
                        outToken = JsonToken::False;
                        currentToken = outToken;
                        return true;
                    }
                    if (outLexeme == "true")
                    {
                        outToken = JsonToken::True;
                        currentToken = outToken;
                        return true;
                    }
                    if (outLexeme == "null")
                    {
                        outToken = JsonToken::Null;
                        currentToken = outToken;
                        return true;
                    }

                    parseError = JsonParseError::InvalidKeyword;
                    errorMessage = "Invalid keyword: " + outLexeme;
                    return false;
                }
            }
        }

        parseError = JsonParseError::OutOfBounds;
        errorMessage = "Input stream out of bounds";

        return false;
    }

    bool JsonReader::NextAvailable()
    {
        if (stream == nullptr)
            return false;
        return !stream->IsOutOfBounds();
    }
    
}
