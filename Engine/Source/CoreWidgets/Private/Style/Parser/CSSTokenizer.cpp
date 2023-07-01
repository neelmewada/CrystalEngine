#include "CoreWidgets.h"

#include <regex>

namespace CE::Widgets
{

	CSSTokenizer::CSSTokenizer(Stream* textStream) : stream(textStream)
	{

	}

	bool CSSTokenizer::HasNextToken()
	{
		return !stream->IsOutOfBounds();
	}

	CSSParserToken CSSTokenizer::NextToken()
	{
		// Regular Expressions
		std::regex identifierRegex = std::regex("[a-zA-Z_][a-zA-Z0-9_-]*");

		if (!HasNextToken())
			return {};

		char c = stream->Read();

		switch (c) {
		case ',':
			return prevToken = { CommaToken, "," };
			break;
		case ':':
			return prevToken = { ColonToken, ":" };
			break;
		case ';':
			return prevToken = { SemiColonToken, ";" };
			break;
		case '{':
			return prevToken = { CurlyOpenToken, "{" };
			break;
		case '}':
			return prevToken = { CurlyCloseToken, "}" };
			break;
		case '[':
			return prevToken = { SquareOpenToken, "[" };
			break;
		case ']':
			return prevToken = { SquareCloseToken, "]" };
			break;
		case '(':
			return prevToken = { ParenOpenToken, "(" };
			break;
		case ')':
			return prevToken = { ParenCloseToken, ")" };
			break;
		case '#':
			return prevToken = { HashSignToken, "#" };
			break;
		case '=':
			return prevToken = { EqualSignToken, "=" };
			break;
		case ' ': case '\n': case '\t':
			if (prevToken.type == IdentifierToken)
				return prevToken = { WhitespaceToken, "" };
			break;
		case '%':
			return prevToken = { PercentageToken, "%" };
		case '\"':
		{
			String stringLiteral = "";
			char prevChar = 0;
			while (!stream->IsOutOfBounds())
			{
				char curChar = stream->Read();
				if (curChar == '\"' && prevChar != '\\')
				{
					break;
				}
				stringLiteral.Append(curChar);
				prevChar = curChar;
			}

			return prevToken = { StringToken, stringLiteral };
		}
			break;
		default:
		{
			if (String::IsNumeric(c) || c == '-' || c == '+') // Numeric literal
			{
				char nextChar = stream->Read();
				if (!String::IsNumeric(nextChar))
				{
					stream->Seek(-1, SeekMode::Current);
					if (c == '-')
						return prevToken = { MinusSignToken, "-" };
					else
						return prevToken = { PlusSignToken, "+" };
				}

				String numberLiteral{};
				numberLiteral.Append(c);

				while (!stream->IsOutOfBounds() && (String::IsNumeric(nextChar) || nextChar == '.'))
				{
					numberLiteral.Append(nextChar);
					nextChar = stream->Read();
				}

				return prevToken = { NumberToken, numberLiteral };
			}

			if (!String::IsAlphabet(c) && c != '_') // NOT an identifier
				break;

			// Identifier
			{
				char nextChar = c;
				std::string identifier{};

				while (!stream->IsOutOfBounds())
				{
					if (!identifier.empty() && !std::regex_match(identifier, identifierRegex))
					{
						identifier = identifier.substr(0, identifier.size() - 1);
						stream->Seek(-1, SeekMode::Current);
						break;
					}

					identifier.append({ nextChar });
					nextChar = stream->Read();
				}

				return prevToken = { IdentifierToken, identifier };
			}
		}
			break;
		}

		return CSSParserToken();
	}

} // namespace CE::Widgets
