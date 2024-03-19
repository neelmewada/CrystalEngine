#include <regex>

#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CSSTokenizer::CSSTokenizer(Stream* cssStream) : stream(cssStream)
	{
		
	}

	bool CSSTokenizer::HasNextToken()
	{
		return !stream->IsOutOfBounds();
	}

#define CASE_TOKEN(x, name) case (#x)[0]: return { CSS::name, #x, line, positionInLine };

	CSSParserToken CSSTokenizer::ParseNextToken()
	{
		if (!HasNextToken())
			return {};

		char c = stream->Read();

		positionInLine++;

		switch (c)
		{
		CASE_TOKEN(., PeriodToken)
		CASE_TOKEN(+, PlusSignToken)
		CASE_TOKEN(-, MinusSignToken)
		CASE_TOKEN(%, PercentageToken)
		CASE_TOKEN(*, AsteriskToken)
		CASE_TOKEN({ , CurlyOpenToken)
		CASE_TOKEN(}, CurlyCloseToken)
		CASE_TOKEN([, SquareOpenToken)
		CASE_TOKEN(], SquareCloseToken)
		CASE_TOKEN(!, ExclamationToken)
		CASE_TOKEN(=, EqualSignToken)
		CASE_TOKEN(> , GreaterThanSign)
		CASE_TOKEN(< , LesserThanSign)
		CASE_TOKEN(;, SemiColonToken)
		CASE_TOKEN(~, TildeToken)
		CASE_TOKEN($, DollarToken)
		CASE_TOKEN(^, CaretToken)
		CASE_TOKEN(|, PipeToken)
		CASE_TOKEN(`, BackquoteToken)
		case ':':
			{
				if (stream->IsOutOfBounds())
					return { CSS::ColonToken, ":", line, positionInLine };
				char next = stream->Read();
				if (next != ':')
				{
					stream->Seek(-1, SeekMode::Current);
					return { CSS::ColonToken, ":", line, positionInLine };
				}
				return { CSS::DoubleColonToken, "::", line, positionInLine };
			}
			break;
		case '#':
			return { CSS::HashSignToken, "#", line, positionInLine };
		case ',':
			return { CSS::CommaToken, ",", line, positionInLine };
		case '(':
			return { CSS::ParenOpenToken, "(", line, positionInLine };
		case ')':
			return { CSS::ParenCloseToken, ")", line, positionInLine };
		case '/': // Check for comment
			{
				if (stream->IsOutOfBounds())
					return { CSS::FwdSlashToken, "/", line, positionInLine };
				char curChar = stream->Read();
				char prevChar = c;
				if (curChar != '*')
				{
					stream->Seek(-1, SeekMode::Current);
					return { CSS::FwdSlashToken, "/", line, positionInLine };
				}
				while (!stream->IsOutOfBounds())
				{
					curChar = stream->Read();
					if (prevChar == '*' && curChar == '/')
						return { CSS::WhitespaceToken, "", line, positionInLine };
					prevChar = curChar;
				}
			}
			break;
		case '\"':
		case '\'':
			{
				String stringLiteral = "";
				char prevChar = 0;
				while (!stream->IsOutOfBounds())
				{
					char curChar = stream->Read();
					if (curChar == c && prevChar != '\\') // ' or " spotted without an escape character
					{
						break;
					}
					stringLiteral.Append(curChar);
					prevChar = curChar;
				}

				return prevToken = { CSS::StringLiteral, stringLiteral, line, positionInLine };
			}
			break;
		case ' ':
		case '\n':
		case '\t':
			if (c == '\n')
			{
				line++;
				positionInLine = 0;
			}
			if (prevToken.type == CSS::IdentifierToken || prevToken.type == CSS::AsteriskToken)
				return prevToken = {CSS::WhitespaceToken, "", line, positionInLine };
			break;
		default:
			if (prevToken.type != CSS::HashSignToken && (String::IsNumeric(c) || c == '-' || c == '+')) // Numeric literal
			{
				char nextChar = stream->Read();
				if (!String::IsNumeric(nextChar) && (nextChar == '-' || nextChar == '+'))
				{
					stream->Seek(-2, SeekMode::Current);
					if (c == '-')
						return prevToken = { CSS::MinusSignToken, "-", line, positionInLine };
					else if (c == '+')
						return prevToken = { CSS::PlusSignToken, "+", line, positionInLine };
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

				return prevToken = { CSS::NumberLiteral, numberLiteral, line, positionInLine };
			}

			if (prevToken.type != CSS::HashSignToken && !String::IsAlphabet(c) && c != '_') // NOT an identifier
				break;
			if (prevToken.type == CSS::HashSignToken && !String::IsNumeric(c) && !String::IsAlphabet(c)) // NOT a color value
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

				return prevToken = { CSS::IdentifierToken, identifier, line, positionInLine };
			}
			break;
		}

		return {};
	}
    
} // namespace CE::Widgets
