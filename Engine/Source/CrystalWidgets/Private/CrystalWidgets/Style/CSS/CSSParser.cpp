#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CSSParser::CSSParser(Stream* stream)
    {
        stream->SetAsciiMode(true);
        stream->SetAutoResizeIncrement(0); // Disable auto resize on stream

        CSSTokenizer tokenizer{ stream };

        while (tokenizer.HasNextToken())
        {
            auto token = tokenizer.ParseNextToken();
            if (token.type == CSS::WhitespaceToken && !tokens.IsEmpty() && (tokens.Top().type == CSS::IdentifierToken || tokens.Top().type == CSS::AsteriskToken))
                tokens.Add(token);
            else if (token.type != CSS::WhitespaceToken)
                tokens.Add(token);
        }
    }

    CSSParser::~CSSParser()
    {
	    
    }

	CSSStyleSheet* CSSParser::ParseStyleSheet(const String& css, Object* owner)
	{
		MemoryStream stream{ css.GetCString(), css.GetLength(), Stream::Permissions::ReadOnly };
		stream.SetAsciiMode(true);
		CSSParser parser{ &stream };

		return parser.ParseStyleSheet(owner);
	}

	CSSStyleSheet* CSSParser::ParseStyleSheet(CSSStyleSheet* original, const String& css)
	{
		MemoryStream stream{ css.GetCString(), css.GetLength(), Stream::Permissions::ReadOnly };
		stream.SetAsciiMode(true);
		CSSParser parser{ &stream };

		parser.ParseStyleSheet(original);
		return original;
	}

	CSSStyleSheet* CSSParser::ParseStyleSheet(Object* owner)
	{
		CSSStyleSheet* stylesheet = CreateObject<CSSStyleSheet>(owner, "StyleSheet");
		if (!ParseStyleSheet(stylesheet))
		{
			stylesheet->RequestDestroy();
			return nullptr;
		}
		return stylesheet;
	}


    bool CSSParser::ParseStyleSheet(CSSStyleSheet* stylesheet)
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

        stylesheet->Clear();

        auto returnFailure = [&](const String& msg, const CSSParserToken& token)
            {
                this->errorMessage = String::Format("CSS Parse Error [line {} position {}]: {}", token.line, token.position, msg);
                CE_LOG(Error, All, errorMessage);
            };

		while (cursor < tokens.GetSize())
		{
			const auto& token = tokens[cursor];
			auto tokenType = token.type;

			if (!insideRule) // Parse selectors
			{
				switch (tokenType)
				{
				case CSS::AsteriskToken:
					if (!curMatchCond.IsValid())
					{
						curMatchCond.matches = CSSSelector::Any;
					}
					break;
				case CSS::WhitespaceToken:
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
				case CSS::IdentifierToken:
					if (tokenStack.IsEmpty())
					{
						curMatchCond.tag = token.lexeme;
						curMatchCond.matches |= CSSSelector::Tag;
						tokenStack.Push(token);
					}
					else if (!insideSquareBraces)
					{
						if (tokenStack.Top().type == CSS::PeriodToken)
						{
							curMatchCond.clazz = token.lexeme;
							curMatchCond.matches |= CSSSelector::Class;
							tokenStack.Push(token);
						}
						else if (tokenStack.Top().type == CSS::HashSignToken)
						{
							curMatchCond.id = token.lexeme;
							curMatchCond.matches |= CSSSelector::Id;
							tokenStack.Push(token);
						}
						else if (tokenStack.Top().type == CSS::ColonToken)
						{
							curMatchCond.states = StateFlagsFromString(token.lexeme);
							curMatchCond.matches |= CSSSelector::State;
							tokenStack.Push(token);
						}
						else if (tokenStack.Top().type == CSS::DoubleColonToken)
						{
							curMatchCond.subControl = SubControlFromString(token.lexeme);
							curMatchCond.matches |= CSSSelector::SubControl;
							tokenStack.Push(token);
						}
					}
					break;
				case CSS::PeriodToken:
					if (tokenStack.NonEmpty() &&
						tokenStack.Top().type != CSS::IdentifierToken && tokenStack.Top().type != CSS::WhitespaceToken && tokenStack.Top().type != CSS::SquareCloseToken)
					{
						returnFailure("Illegal period", token);
						return false;
					}
					tokenStack.Push(token);
					break;
				case CSS::HashSignToken:
					if (tokenStack.NonEmpty() &&
						tokenStack.Top().type != CSS::IdentifierToken && tokenStack.Top().type != CSS::WhitespaceToken && tokenStack.Top().type != CSS::SquareCloseToken)
					{
						returnFailure("Illegal hash sign #", token);
						return false;
					}
					tokenStack.Push(token);
					break;
				case CSS::ColonToken:
					if (tokenStack.NonEmpty() &&
						tokenStack.Top().type != CSS::IdentifierToken && tokenStack.Top().type != CSS::WhitespaceToken && tokenStack.Top().type != CSS::SquareCloseToken)
					{
						returnFailure("Illegal colon", token);
						return false;
					}
					tokenStack.Push(token);
					break;
				case CSS::DoubleColonToken:
					if (tokenStack.NonEmpty() &&
						tokenStack.Top().type != CSS::IdentifierToken && tokenStack.Top().type != CSS::WhitespaceToken && tokenStack.Top().type != CSS::SquareCloseToken)
					{
						returnFailure("Illegal double colon", token);
						return false;
					}
					tokenStack.Push(token);
					break;
				case CSS::CommaToken:
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
				case CSS::SemiColonToken:
					break;
				case CSS::EqualSignToken:
					break;
				case CSS::PlusSignToken:
					break;
				case CSS::MinusSignToken:
					break;
				case CSS::PercentageToken:
					break;
				case CSS::CurlyOpenToken:
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
				case CSS::CurlyCloseToken:

					insideRule = false;
					break;
				case CSS::SquareOpenToken:
					insideSquareBraces = true;
					break;
				case CSS::SquareCloseToken:
					insideSquareBraces = false;
					break;
				case CSS::ParenOpenToken:
					break;
				case CSS::ParenCloseToken:
					break;
				case CSS::StringLiteral:
					break;
				case CSS::NumberLiteral:
					break;
				case CSS::ExclamationToken:
					break;
				default:
					break;
				}
			}
			else // Parse properties
			{
				switch (tokenType)
				{
				case CSS::CurlyCloseToken:
				{
					insideRule = false;
					stylesheet->rules.Top().style = curStyle;
					tokenStack.Clear();
					curStyle = {};
					curProperty = {};
					curPropertyValue = {};
					curSelectorList = {};
				}
				break;
				case CSS::IdentifierToken:
					if (tokenStack.IsEmpty() && curProperty == CStylePropertyType::None)
					{
						curProperty = StylePropertyTypeFromString(token.lexeme);
						tokenStack.Push(token);
					}
					break;
				case CSS::ColonToken:
				{
					CSS::TokenType prevToken = CSS::ColonToken;
					cursor++;
					ParsePropertyValue(cursor, curProperty, curPropertyValue);
					if (curPropertyValue.IsValid())
						curStyle.properties[curProperty] = curPropertyValue;
					tokenStack.Clear();
					curProperty = {};
					curPropertyValue = {};
				}
				break;
				case CSS::SemiColonToken:
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

	    return true;
    }

	static HashMap<String, Color> colorIdentifiersMap{
		{ "white", Color::White() },
		{ "black", Color::Black() },
		{ "red", Color::Red() },
		{ "green", Color::Green() },
		{ "blue", Color::Blue() },
		{ "yellow", Color::RGBA8(255, 255, 0) },
		{ "cyan", Color::RGBA8(0, 255, 255) },
		{ "pink", Color::RGBA8(255, 0, 255) },
		{ "orange", Color::RGBA8(255, 128, 0) },
		{ "teal", Color::RGBA8(0, 128, 128) },
		{ "gray", Color::RGBA8(128, 128, 128) },
	};

	void CSSParser::ParsePropertyValue(int& cursor, CStylePropertyType property, CStyleValue& outValue)
	{
		auto avail = [&]() -> bool { return cursor < tokens.GetSize(); };
		auto read = [&]() -> const CSSParserToken& { return tokens[cursor]; };
		auto skip = [&]() {
			while (cursor < tokens.GetSize() && tokens[cursor].type != CSS::SemiColonToken)
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

		while (current->type == CSS::WhitespaceToken && avail())
		{
			current = &tokens[cursor];
			cursor++;
		}

		if (!avail())
			return;

		if (current->type == CSS::IdentifierToken)
		{
			if (current->lexeme == "url")
			{
				String url = "";

				current = &tokens[cursor];
				if (current->type == CSS::WhitespaceToken)
				{
					cursor++;
					current = &tokens[cursor];
				}
				if (!avail() || current->type != CSS::ParenOpenToken)
				{
					returnFailure("No parenthesis found after rgb/rgba token", current);
					return;
				}

				cursor++;
				bool endReached = false;
				while (cursor < tokens.GetSize())
				{
					if (tokens[cursor].type == CSS::ParenCloseToken)
					{
						endReached = true;
					}
					else if (tokens[cursor].type == CSS::SemiColonToken)
					{
						break;
					}

					if (!endReached)
						url += tokens[cursor].lexeme;
					cursor++;
				}

				outValue = String(url);
			}
			else if (current->lexeme == "rgb" || current->lexeme == "rgba")
			{
				Color color{};
				color.a = 1.0f;

				current = &tokens[cursor];
				if (current->type == CSS::WhitespaceToken)
				{
					cursor++;
					current = &tokens[cursor];
				}
				if (!avail() || current->type != CSS::ParenOpenToken)
				{
					returnFailure("No parenthesis found after rgb/rgba token", current);
					return;
				}

				cursor++;
				int startCursor = cursor;
				int endCursor = cursor;

				while (cursor < tokens.GetSize())
				{
					if (tokens[cursor].type == CSS::ParenCloseToken)
					{
						endCursor = cursor - 1;
					}
					else if (tokens[cursor].type == CSS::SemiColonToken)
					{
						break;
					}
					cursor++;
					current = &tokens[cursor];
				}

				if (startCursor < endCursor)
				{
					Vec4 vec{ 0, 0, 0, 1 };
					int idx = 0;
					for (int cursor = startCursor; cursor <= endCursor && idx < 4; cursor++)
					{
						if (tokens[cursor].type == CSS::NumberLiteral)
						{
							vec.xyzw[idx++] = String::Parse<f32>(tokens[cursor].lexeme) / 255.0f;
						}
					}
					outValue = Color(vec.x, vec.y, vec.z, vec.w);
				}
			}
			else // Enum value or color
			{
				if (colorIdentifiersMap.KeyExists(current->lexeme))
				{
					outValue = colorIdentifiersMap[current->lexeme];
				}
				else if (current->lexeme == "auto")
				{
					outValue.enumValue = Vec4i(1, 1, 1, 1) * CStyleValue::Auto;
					outValue.valueType = CStyleValue::Type_Enum;
				}
				else if (current->lexeme == "none")
				{
					outValue.enumValue = Vec4i(1, 1, 1, 1) * CStyleValue::None;
					outValue.valueType = CStyleValue::Type_Enum;
				}
				else
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
		}
		else if (current->type == CSS::HashSignToken) // Color: #01010101
		{
			current = &tokens[cursor];
			int len = current->lexeme.GetLength();
			if (current->type != CSS::IdentifierToken && current->type != CSS::NumberLiteral)
			{
				returnFailure("Invalid token. Expected a hex color value", current);
				return;
			}
			Color col{};
			col.a = 1.0f;
			ParseColorU32(cursor, col);
			outValue = col;
		}
		else if (current->type == CSS::NumberLiteral) // Single or Vector
		{
			cursor--;
			current = &tokens[cursor];
			int startCursor = cursor;
			int endCursor = cursor;

			while (cursor < tokens.GetSize())
			{
				if (tokens[cursor].type == CSS::SemiColonToken)
				{
					endCursor = cursor - 1;
					break;
				}
				cursor++;
				current = &tokens[cursor];
			}

			Vec4 value{};
			int count = 0;
			bool isPercent = false;
			Vec4i enumValue{};
			int curIdx = 0;

			for (int i = startCursor; i <= endCursor; i++)
			{
				if (tokens[i].type == CSS::WhitespaceToken)
					continue;

				if (tokens[i].type == CSS::NumberLiteral && curIdx < 4)
				{
					value.xyzw[curIdx++] = String::Parse<f32>(tokens[i].lexeme);
				}
				else if (tokens[i].type == CSS::IdentifierToken && tokens[i].lexeme == "auto" && curIdx < 4)
				{
					enumValue[curIdx] = CStyleValue::Auto;
				}
				else if (tokens[i].type == CSS::PercentageToken && curIdx > 0 && curIdx <= 4)
				{
					isPercent = true;
					enumValue.xyzw[curIdx - 1] = CStyleValue::Percent;
				}
			}

			if (curIdx == 1) // Single
			{
				outValue = CStyleValue(value.x, isPercent);
			}
			else if (curIdx == 2) // Vec2
			{
				f32 horizontal = value.x;
				f32 vertical = value.y;
				outValue = Vec4(horizontal, vertical, horizontal, vertical);
				outValue.enumValue = enumValue;
			}
			else if (curIdx == 4) // Vec4
			{
				outValue = value;
				outValue.enumValue = enumValue;
			}
		}
		else if (current->type == CSS::StringLiteral)
		{
			outValue = current->lexeme;
		}
	}

	void CSSParser::ParseVector(int startCursor, int endCursor, Vec4& out)
	{
		int idx = 0;
		for (int cursor = startCursor; cursor <= endCursor && idx < 4; cursor++)
		{
			if (tokens[cursor].type == CSS::NumberLiteral)
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
		out = Color::RGBAHex(value);
	}

    
} // namespace CE::Widgets
