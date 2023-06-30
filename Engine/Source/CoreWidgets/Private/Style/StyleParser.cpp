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
        std::regex identifierRegex = std::regex("[a-zA-Z_][a-zA-Z0-9_]*");
        std::regex propertyRegex = std::regex("[a-zA-Z_][a-zA-Z0-9_-]*");
        
        int length = source.GetLength();
        
        for (int cursor = 0; cursor < source.GetLength(); cursor++)
        {
            char c = source[cursor];
            std::string_view substring(source.GetCString() + cursor, length - cursor - 1);
            
            switch (source[cursor]) {
                case ':':
                    tokens.Add({CSSTokenType::Colon, ":"});
                    break;
                default:
                    break;
            }
            
        }
    }
    
} // namespace CE::Widgets
