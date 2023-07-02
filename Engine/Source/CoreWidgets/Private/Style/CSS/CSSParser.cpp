#include "CoreMinimal.h"

#include <string>
#include <regex>

namespace CE::Widgets
{

	CSSParser::CSSParser(Stream* stream)
	{
		stream->SetAsciiMode(true);

		CSSTokenizer tokenizer{ stream };

		while (tokenizer.HasNextToken())
		{
			auto token = tokenizer.NextToken();
			if (token.type == WhitespaceToken && !tokens.IsEmpty() && tokens.Top().type == IdentifierToken)
				tokens.Add(token);
			else if (token.type != WhitespaceToken)
				tokens.Add(token);
		}
	}

	CSSParser::~CSSParser()
	{

	}

	CSSStyleSheet* CSSParser::ParseStyleSheet(const String& css, CSSStyleSheet* parent)
	{
		MemoryStream stream{ css.GetCString(), css.GetLength(), Stream::Permissions::ReadOnly };
		stream.SetAsciiMode(true);
		CSSParser parser{ &stream };

		return parser.ParseStyleSheet(parent);
	}

	CSSStyleSheet* CSSParser::ParseStyleSheet(CSSStyleSheet* parent)
	{
		CSSSelector::MatchCond curMatchCond{};
		CSSSelector curSelector{};
		CSSSelectorList curSelectorList{};
		int cursor = 0;

		Array<CSSParserToken> tokenStack{};

		bool insideRule = false;
		bool insideSquareBraces = false;

		CSSStyleSheet* stylesheet = CreateObject<CSSStyleSheet>(parent, "StyleSheet");

		auto returnFailure = [&](const String& msg, const CSSParserToken& token)
		{
			CE_LOG(Error, All, "CSS Parse Error [line {} position {}]: {}", token.line, token.position, msg);
			stylesheet->RequestDestroy();
			stylesheet = nullptr;
		};

		while (cursor < tokens.GetSize())
		{
			const auto& token = tokens[cursor];
			auto tokenType = token.type;

			if (!insideRule) // Parse selectors
			{
				switch (tokenType)
				{
				case WhitespaceToken:
					if (!insideSquareBraces)
					{
						if (curMatchCond.IsValid())
						{
							curSelector.primary = curMatchCond;
							curSelector.relation = CSSSelector::Descendent;
							curMatchCond = {};
						}
						tokenStack.Clear();
					}
					break;
				case IdentifierToken:
					if (tokenStack.IsEmpty())
					{
						curMatchCond.tag = token.lexeme;
						curMatchCond.matches |= CSSSelector::Tag;
						tokenStack.Push(token);
					}
					else if (!insideSquareBraces)
					{
						if (tokenStack.Top().type == PeriodToken)
						{
							curMatchCond.clazz = token.lexeme;
							curMatchCond.matches |= CSSSelector::Class;
							tokenStack.Push(token);
						}
						else if (tokenStack.Top().type == HashSignToken)
						{
							curMatchCond.id = token.lexeme;
							curMatchCond.matches |= CSSSelector::Id;
							tokenStack.Push(token);
						}
						else if (tokenStack.Top().type == ColonToken)
						{
							curMatchCond.states = StateFlagsFromString(token.lexeme);
							curMatchCond.matches |= CSSSelector::State;
							tokenStack.Push(token);
						}
						else if (tokenStack.Top().type == DoubleColonToken)
						{
							curMatchCond.subControl = SubControlFromString(token.lexeme);
							curMatchCond.matches |= CSSSelector::SubControl;
							tokenStack.Push(token);
						}
					}
					break;
				case PeriodToken:
					if (tokenStack.NonEmpty() && 
						tokenStack.Top().type != IdentifierToken && tokenStack.Top().type != WhitespaceToken && tokenStack.Top().type != SquareCloseToken)
					{
						returnFailure("Illegal period", token);
						return nullptr;
					}
					tokenStack.Push(token);
					break;
				case HashSignToken:
					if (tokenStack.NonEmpty() &&
						tokenStack.Top().type != IdentifierToken && tokenStack.Top().type != WhitespaceToken && tokenStack.Top().type != SquareCloseToken)
					{
						returnFailure("Illegal hash sign #", token);
						return nullptr;
					}
					tokenStack.Push(token);
					break;
				case ColonToken:
					if (tokenStack.NonEmpty() &&
						tokenStack.Top().type != IdentifierToken && tokenStack.Top().type != WhitespaceToken && tokenStack.Top().type != SquareCloseToken)
					{
						returnFailure("Illegal colon", token);
						return nullptr;
					}
					tokenStack.Push(token);
					break;
				case DoubleColonToken:
					if (tokenStack.NonEmpty() &&
						tokenStack.Top().type != IdentifierToken && tokenStack.Top().type != WhitespaceToken && tokenStack.Top().type != SquareCloseToken)
					{
						returnFailure("Illegal double colon", token);
						return nullptr;
					}
					tokenStack.Push(token);
					break;
				case CommaToken:
					if (!insideSquareBraces)
					{
						if (curMatchCond.IsValid())
						{
							curSelector.primary = curMatchCond;
							curSelector.relation = CSSSelector::Descendent;
							curMatchCond = {};
						}
						tokenStack.Clear();

						if (curSelector.primary.IsValid())
						{
							curSelectorList.Add(curSelector);
							curSelector = {};
							curMatchCond = {};
						}
					}
					tokenStack.Clear();
					break;
				case SemiColonToken:
					break;
				case EqualSignToken:
					break;
				case AsteriskToken:
					break;
				case PlusSignToken:
					break;
				case MinusSignToken:
					break;
				case PercentageToken:
					break;
				case CurlyOpenToken:
					if (curSelector.primary.IsValid())
					{
						curSelectorList.Add(curSelector);
					}
					stylesheet->rules.Add({});
					stylesheet->rules.Top().selectorList = curSelectorList;

					curMatchCond = {};
					curSelector = {};
					curSelectorList = {};
					tokenStack.Clear();
					insideRule = true;
					insideSquareBraces = false;
					break;
				case CurlyCloseToken:

					insideRule = false;
					break;
				case SquareOpenToken:
					insideSquareBraces = true;
					break;
				case SquareCloseToken:
					insideSquareBraces = false;
					break;
				case ParenOpenToken:
					break;
				case ParenCloseToken:
					break;
				case StringToken:
					break;
				case NumberToken:
					break;
				case HyphenToken:
					break;
				case ExclamationToken:
					break;
				default:
					break;
				}
			}
			else // Parse properties
			{
				if (tokenType == CurlyCloseToken)
				{
					insideRule = false;
				}
			}

			cursor++;
		}

		return stylesheet;
	}

} // namespace CE::Widgets
