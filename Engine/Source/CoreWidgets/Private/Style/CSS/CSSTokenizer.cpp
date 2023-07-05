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

		if (!HasNextToken())
			return {};

		positionInLine++;

		char c = stream->Read();

		switch (c) {
		case '.':
			return prevToken = { PeriodToken, ".", line, positionInLine };
		case ',':
			return prevToken = { CommaToken, ",", line, positionInLine };
		case ':':
			if (!stream->IsOutOfBounds())
			{
				char next = stream->Read();
				if (next == ':')
					return prevToken = { DoubleColonToken, "::", line, positionInLine };
				stream->Seek(-1, SeekMode::Current);
			}
			return prevToken = { ColonToken, ":", line, positionInLine };
		case ';':
			return prevToken = { SemiColonToken, ";", line, positionInLine };
		case '{':
			return prevToken = { CurlyOpenToken, "{", line, positionInLine };
		case '}':
			return prevToken = { CurlyCloseToken, "}", line, positionInLine };
		case '[':
			return prevToken = { SquareOpenToken, "[", line, positionInLine };
		case ']':
			return prevToken = { SquareCloseToken, "]", line, positionInLine };
		case '(':
			return prevToken = { ParenOpenToken, "(", line, positionInLine };
		case ')':
			return prevToken = { ParenCloseToken, ")", line, positionInLine };
		case '#':
			return prevToken = { HashSignToken, "#", line, positionInLine };
		case '*':
			return prevToken = { AsteriskToken, "*", line, positionInLine };
		case '=':
			return prevToken = { EqualSignToken, "=", line, positionInLine };
		case ' ': case '\n': case '\t':
			if (c == '\n')
			{
				line++;
				positionInLine = 0;
			}
			if (prevToken.type == IdentifierToken || prevToken.type == AsteriskToken)
				return prevToken = { WhitespaceToken, "", line, positionInLine };
			break;
		case '%':
			return prevToken = { PercentageToken, "%", line, positionInLine };
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

			return prevToken = { StringToken, stringLiteral, line, positionInLine };
		}
			break;
		default:
		{
			if (prevToken.type != HashSignToken && (String::IsNumeric(c) || c == '-' || c == '+')) // Numeric literal
			{
				char nextChar = stream->Read();
				if (!String::IsNumeric(nextChar) && (nextChar == '-' || nextChar == '+'))
				{
					stream->Seek(-2, SeekMode::Current);
					if (c == '-')
						return prevToken = { MinusSignToken, "-", line, positionInLine };
					else
						return prevToken = { PlusSignToken, "+", line, positionInLine };
				}

				String numberLiteral{};
				numberLiteral.Append(c);

				while (!stream->IsOutOfBounds())
				{
					if (!String::IsNumeric(nextChar) && nextChar != '.')
					{
						stream->Seek(-1, SeekMode::Current);
						break;
					}
					numberLiteral.Append(nextChar);
					nextChar = stream->Read();
				}

				return prevToken = { NumberToken, numberLiteral, line, positionInLine };
			}

			if (prevToken.type != HashSignToken && !String::IsAlphabet(c) && c != '_') // NOT an identifier
				break;
			if (prevToken.type == HashSignToken && !String::IsNumeric(c) && !String::IsAlphabet(c)) // NOT a color value
				break;

			// Identifier
			{
				char nextChar = c;
				std::string identifier{};
				std::regex identifierRegex = std::regex("[a-zA-Z0-9_][a-zA-Z0-9_-]*");

				while (!stream->IsOutOfBounds())
				{
					if (!identifier.empty() && !std::regex_match(identifier, identifierRegex))
					{
						identifier = identifier.substr(0, identifier.size() - 1);
						stream->Seek(-2, SeekMode::Current);
						break;
					}

					identifier.append({ nextChar });
					nextChar = stream->Read();
				}

				return prevToken = { IdentifierToken, identifier, line, positionInLine };
			}
		}
			break;
		}

		return CSSParserToken();
	}

} // namespace CE::Widgets
