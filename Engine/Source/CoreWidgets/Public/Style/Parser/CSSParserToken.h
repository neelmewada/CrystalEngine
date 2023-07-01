#pragma once

namespace CE::Widgets
{

	enum CSSParserTokenType
	{
		WhitespaceToken = 0,
		IdentifierToken,
		ColonToken,
		SemiColonToken,
		CommaToken,
		HashSignToken,
		EqualSignToken,
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
	};
    
} // namespace CE::Widgets
