#pragma once

namespace CE::Widgets
{
    namespace CSS
    {
        enum TokenType
        {
            WhitespaceToken = 0,
            IdentifierToken,
            DoubleColonToken,
            ColonToken,
            SemiColonToken,
            CommaToken,
            HashSignToken,
            EqualSignToken,
            GreaterThanSign, LesserThanSign,
            AsteriskToken,
            PeriodToken,
            PlusSignToken, MinusSignToken,
            PercentageToken,
            FwdSlashToken,
            CurlyOpenToken, CurlyCloseToken,
            SquareOpenToken, SquareCloseToken,
            ParenOpenToken, ParenCloseToken,
            StringLiteral,
            NumberLiteral,
            ExclamationToken,
            TildeToken,
            DollarToken,
            PipeToken,
            BackquoteToken,
            CaretToken,
        };
    } // namespace CSS

    struct CSSParserToken
    {
        CSS::TokenType type{};
        String lexeme{};
        int line = 0;
        int position = 0;
    };
    
} // namespace CE::Widgets
