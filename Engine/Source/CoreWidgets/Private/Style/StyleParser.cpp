#include "CoreMinimal.h"

#include <string>
#include <regex>

namespace CE::Widgets
{

    StyleParser::StyleParser()
    {
        
    }

    StyleParser::~StyleParser()
    {

    }

    void StyleParser::ParseTokens(const String& source)
    {
        tokens.Clear();
        
        // Regular Expressions
        std::regex identifierRegex = std::regex("[a-zA-Z_][a-zA-Z0-9_-]*");
        
        int length = source.GetLength();
        
        for (int cursor = 0; cursor < source.GetLength(); cursor++)
        {
            char c = source[cursor];
            std::string_view substring(source.GetCString() + cursor, length - cursor - 1);
            
            switch (source[cursor]) {
				case ',':
					tokens.Add({ CSSTokenType::Comma, "," });
					break;
                case ':':
					tokens.Add({ CSSTokenType::Colon, ":" });
                    break;
				case ';':
					tokens.Add({ CSSTokenType::SemiColon, ";" });
					break;
				case '{':
					tokens.Add({ CSSTokenType::CurlyOpen, "{" });
					break;
				case '}':
					tokens.Add({ CSSTokenType::CurlyClose, "}" });
					break;
				case '[':
					tokens.Add({ CSSTokenType::SquareOpen, "[" });
					break;
				case ']':
					tokens.Add({ CSSTokenType::SquareClose, "]" });
					break;
				case '(':
					tokens.Add({ CSSTokenType::ParenOpen, "(" });
					break;
				case ')':
					tokens.Add({ CSSTokenType::ParenClose, ")" });
					break;
				case '#':
					tokens.Add({ CSSTokenType::HashSign, "#" });
					break;
				case '=':
					tokens.Add({ CSSTokenType::EqualSign, "=" });
					break;
				case ' ': case '\n': case '\t':
					if (tokens.NonEmpty() && tokens.Top().type == CSSTokenType::Identifier)
						tokens.Add({ CSSTokenType::Whitespace, " " });
					break;
				case '\"':
				{
					int endCursor = cursor;
					while (endCursor < length)
					{
						if (source[endCursor] == '\"' && (endCursor == 0 || source[endCursor - 1] != '\\'))
						{
							break;
						}
						endCursor++;
					}
					if (endCursor > cursor)
						endCursor--;

					if (cursor == endCursor)
						tokens.Add({ CSSTokenType::StringLiteral, "" });
					else
						tokens.Add({ CSSTokenType::StringLiteral, String(std::string(source.GetCapacity() + cursor + 1, endCursor - cursor + 1)) });

				}
					break;
                default:
				{
					int endCursor = cursor;

					if (!String::IsAlphabet(c) && c != '_') // NOT an identifier
					{
						if (String::IsNumeric(c) || (source[cursor] == '-' && cursor < length - 1 && String::IsNumeric(source[cursor + 1]))) // Numeric literal
						{
							if (source[endCursor] == '-') endCursor++;
							while (endCursor < length && (source[endCursor] == '.' || String::IsNumeric(source[endCursor])))
							{
								endCursor++;
							}
							if (endCursor > cursor)
								endCursor--;

							tokens.Add({ CSSTokenType::NumberLiteral, String(std::string(source.GetCString() + cursor, endCursor - cursor + 1)) });

							cursor = endCursor;
						}

						break;
					}

					// Wait till identifier/keyword is valid
					while (endCursor < length &&
						(String::IsAlphabet(source[endCursor]) || String::IsNumeric(source[endCursor]) || source[endCursor] == '_' || source[endCursor] == '-'))
					{
						endCursor++;
					}
					if (endCursor > cursor)
						endCursor--;

					std::string_view tokenView(source.GetCString() + cursor, endCursor - cursor + 1);
					std::string tokenString = std::string(tokenView);

					if (std::regex_match(tokenString, identifierRegex)) // Identifier
					{
						tokens.Add({ CSSTokenType::Identifier, tokenString });
					}

					cursor = endCursor;

				}
                    break;
            }
            
        }
    }
    
} // namespace CE::Widgets
