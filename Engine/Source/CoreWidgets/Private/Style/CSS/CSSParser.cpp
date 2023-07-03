#include "CoreMinimal.h"

#include <string>
#include <regex>

namespace CE::Widgets
{
	using namespace CE::Widgets::Internal;

	CSSParser::CSSParser(Stream* stream)
	{
		stream->SetAsciiMode(true);

		CSSTokenizer tokenizer{ stream };

		while (tokenizer.HasNextToken())
		{
			auto token = tokenizer.NextToken();
			if (token.type == WhitespaceToken && !tokens.IsEmpty() && (tokens.Top().type == IdentifierToken || tokens.Top().type == AsteriskToken))
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

		CStyle curStyle{};
		CStylePropertyType curProperty = CStylePropertyType::None;
		CStyleValue curPropertyValue{};

		bool insideRule = false;
		bool insideSquareBraces = false;
		//bool primaryCondSet = false;

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
				case AsteriskToken:
					if (!curMatchCond.IsValid())
					{
						curMatchCond.matches = CSSSelector::Any;
					}
					break;
				case WhitespaceToken:
					if (!insideSquareBraces)
					{
						if (curMatchCond.IsValid() && !curSelector.primary.IsValid())
						{
							curSelector.primary = curMatchCond;
							curSelector.relation = CSSSelector::Descendent;
							curMatchCond = {};
						}
						else if (curMatchCond.IsValid() && curSelector.primary.IsValid())
						{
							curSelector.secondary = curMatchCond;
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
						if (curMatchCond.IsValid() && !curSelector.primary.IsValid())
						{
							curSelector.primary = curMatchCond;
							curSelector.relation = CSSSelector::Descendent;
							curMatchCond = {};
						}
						else if (curMatchCond.IsValid() && curSelector.primary.IsValid())
						{
							curSelector.secondary = curMatchCond;
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
				case PlusSignToken:
					break;
				case MinusSignToken:
					break;
				case PercentageToken:
					break;
				case CurlyOpenToken:
					if (!curSelector.primary.IsValid() && curMatchCond.IsValid())
					{
						curSelector.primary = curMatchCond;
						curMatchCond = {};
					}
					else if (curSelector.primary.IsValid() && curMatchCond.IsValid())
					{
						curSelector.secondary = curMatchCond;
						curMatchCond = {};
					}
					if (curSelector.primary.IsValid())
					{
						curSelectorList.Add(curSelector);
					}
					curSelectorList.CalculateHash();
					stylesheet->rules.Add({});
					stylesheet->rules.Top().selectorList = curSelectorList;

					curMatchCond = {};
					curSelector = {};
					curSelectorList = {};
					tokenStack.Clear();
					insideRule = true;
					insideSquareBraces = false;
					curStyle = {};
					curProperty = {};
					curPropertyValue = {};
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
				switch (tokenType)
				{
				case CurlyCloseToken:
				{
					insideRule = false;
					stylesheet->rules.Top().style = curStyle;

					curStyle = {};
					curProperty = {};
					curPropertyValue = {};
					curSelectorList = {};
				}
					break;
				case IdentifierToken:
					if (tokenStack.IsEmpty() && curProperty == CStylePropertyType::None)
					{
						curProperty = StylePropertyTypeFromString(token.lexeme);
						tokenStack.Push(token);
					}
					break;
				case ColonToken:
				{
					CSSParserTokenType prevToken = ColonToken;
					cursor++;
					ParsePropertyValue(cursor, curProperty, curPropertyValue);
					if (curPropertyValue.IsValid())
						curStyle.properties[curProperty] = curPropertyValue;
					tokenStack.Clear();
					curProperty = {};
					curPropertyValue = {};
				}
					break;
				case SemiColonToken:
					tokenStack.Clear();
					curProperty = {};
					curPropertyValue = {};
					break;
				default:
					break;
				}
			}

			cursor++;
		}

		return stylesheet;
	}

	void CSSParser::ParsePropertyValue(int& cursor, CStylePropertyType property, CStyleValue& outValue)
	{
		auto avail = [&]() -> bool { return cursor < tokens.GetSize(); };
		auto read = [&]() -> const CSSParserToken& { return tokens[cursor]; };
		auto skip = [&]() {
			while (cursor < tokens.GetSize() && tokens[cursor].type != SemiColonToken)
			{
				cursor++;
			}
		};
		auto returnFailure = [](const String& msg, const CSSParserToken* token)
		{
			CE_LOG(Error, All, "CSS Parse Error [line {} position {}]: {}", token->line, token->position, msg);
		};

		const CSSParserToken* current = nullptr;

		if (!avail())
			return;
		current = &tokens[cursor];
		cursor++;

		while (current->type == WhitespaceToken && avail())
		{
			current = &tokens[cursor];
			cursor++;
		}

		if (!avail())
			return;

		if (current->type == IdentifierToken)
		{
			if (current->lexeme == "rgb" || current->lexeme == "rgba")
			{
				Color color{};
				color.a = 1.0f;

				current = &tokens[cursor];
				if (current->type == WhitespaceToken)
				{
					cursor++;
					current = &tokens[cursor];
				}
				if (!avail() || current->type != ParenOpenToken)
				{
					returnFailure("No parenthesis found after rgb/rgba token", current);
					return;
				}

				cursor++;
				int startCursor = cursor;
				int endCursor = cursor;

				while (cursor < tokens.GetSize())
				{
					if (tokens[cursor].type == ParenCloseToken)
					{
						endCursor = cursor - 1;
					}
					else if (tokens[cursor].type == SemiColonToken)
					{
						break;
					}
					cursor++;
					current = &tokens[cursor];
				}

				if (startCursor < endCursor)
				{
					Vec4 vec{};
					ParseVector(startCursor, endCursor, vec);
					outValue = Color(vec.x, vec.y, vec.z, vec.w);
				}
			}
			else // Enum value
			{
				EnumType* enumType = GetEnumTypeForProperty(property);
				if (enumType != nullptr)
				{
					String pascalCase = current->lexeme.ToPascalCase();
					outValue.enumValue = Vec4i(1, 1, 1, 1) * (s32)enumType->GetConstantValue(pascalCase);
					outValue.valueType = CStyleValue::Type_Enum;
				}
			}
		}
		else if (current->type == HashSignToken) // Color: #01010101
		{
			current = &tokens[cursor];
			int len = current->lexeme.GetLength();
			if (current->type != IdentifierToken && current->type != NumberToken)
			{
				returnFailure("Invalid token. Expected a hex color value", current);
				return;
			}
			Color col{};
			col.a = 1.0f;
			ParseColorU32(cursor, col);
			outValue = col;
		}
		else if (current->type == NumberToken) // Single or Vector
		{
			cursor--;
			current = &tokens[cursor];
			int startCursor = cursor;
			int endCursor = cursor;

			while (current->type == NumberToken)
			{
				cursor++;
				current = &tokens[cursor];
			}

			endCursor = cursor - 1;

			if (startCursor == endCursor) // Single
			{
				outValue = CStyleValue(String::Parse<f32>(tokens[startCursor].lexeme));
			}
			else if (startCursor + 1 == endCursor) // Vec2
			{
				f32 horizontal = String::Parse<f32>(tokens[startCursor].lexeme); 
				f32 vertical = String::Parse<f32>(tokens[endCursor].lexeme);
				outValue = Vec4(horizontal, vertical, horizontal, vertical);
			}
			else if (startCursor + 3 == endCursor) // Vec4
			{
				f32 x = String::Parse<f32>(tokens[startCursor].lexeme);
				f32 y = String::Parse<f32>(tokens[startCursor + 1].lexeme);
				f32 z = String::Parse<f32>(tokens[startCursor + 2].lexeme);
				f32 w = String::Parse<f32>(tokens[startCursor + 3].lexeme);
				outValue = Vec4(x, y, z, w);
			}
		}
	}

	void CSSParser::ParseVector(int startCursor, int endCursor, Vec4& out)
	{
		int idx = 0;
		for (int cursor = startCursor; cursor <= endCursor && idx < 4; cursor++)
		{
			if (tokens[cursor].type == NumberToken)
			{
				out.xyzw[idx++] = String::Parse<f32>(tokens[cursor].lexeme);
			}
		}
	}

	void CSSParser::ParseColorU32(int cursor, Color& out)
	{
		const auto& lexeme = tokens[cursor].lexeme;
		int len = lexeme.GetLength();
		u32 value = 0;
		sscanf(lexeme.GetCString(), "%x", &value);
		out = Color::FromRGBAHex(value);
	}

} // namespace CE::Widgets
