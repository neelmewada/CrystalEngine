#pragma once

namespace CE::Widgets
{

    enum class CSSTokenType
    {
        Whitespace = 0,
        Identifier,
        Colon,
        SemiColon,
        CurlyOpen, CurlyClose,
        SquareOpen, SquareClose,
        ParenOpen, ParenClose,
        StringLiteral,
        NumberLiteral,
        Hyphen, Exclamation,
    };
    ENUM_CLASS_FLAGS(CSSTokenType);

    struct CSSToken
    {
        CSSTokenType type{};
        String lexeme{};
    };
    
    class COREWIDGETS_API StyleParser
    {
    public:
        
        StyleParser();
        ~StyleParser();
        
        void ParseTokens(const String& css);
        
    private:
        
        Array<CSSToken> tokens{};
    };

} // namespace CE::Widgets

