#pragma once

namespace CE::Widgets
{

	enum CSSParserTokenType
	{
		WhitespaceToken = 0,
		IdentifierToken,
		DoubleColonToken,
		ColonToken,
		SemiColonToken,
		CommaToken,
		HashSignToken,
		EqualSignToken,
		AsteriskToken,
		PeriodToken,
		PlusSignToken, MinusSignToken,
		PercentageToken,
		CurlyOpenToken, CurlyCloseToken,
		SquareOpenToken, SquareCloseToken,
		ParenOpenToken, ParenCloseToken,
		StringToken,
		NumberToken,
		HyphenToken, ExclamationToken,
	};
	ENUM_CLASS_FLAGS(CSSParserTokenType);

	struct CSSParserToken
	{
		CSSParserTokenType type{};
		String lexeme{};
		int line = 0;
		int position = 0;
	};
    
} // namespace CE::Widgets
