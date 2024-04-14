#include "CrystalWidgets.h"

// ReSharper disable StringLiteralTypo

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

		CStyleSheet::MarkAllDirty();

	    return true;
    }

	static HashMap<String, Color> colorIdentifiersMap{
		{ "black", Color::RGBHex(0x000000) },
		{ "silver", Color::RGBHex(0xc0c0c0) },
		{ "gray", Color::RGBHex(0x808080) },
		{ "white", Color::RGBHex(0xffffff) },
		{ "maroon", Color::RGBHex(0x800000) },
		{ "red", Color::RGBHex(0xff0000) },
		{ "purple", Color::RGBHex(0x800080) },
		{ "fuchsia", Color::RGBHex(0xff00ff) },
		{ "green", Color::RGBHex(0x008000) },
		{ "lime", Color::RGBHex(0x00ff00) },
		{ "olive", Color::RGBHex(0x808000) },
		{ "yellow", Color::RGBHex(0xffff00) },
		{ "navy", Color::RGBHex(0x000080) },
		{ "blue", Color::RGBHex(0x0000ff) },
		{ "teal", Color::RGBHex(0x008080) },
		{ "aqua", Color::RGBHex(0x00ffff) },
		{ "orange", Color::RGBHex(0xffa500) },
		{ "aliceblue", Color::RGBHex(0xf0f8ff) },
		{ "antiquewhite", Color::RGBHex(0xfaebd7) },
		{ "aquamarine", Color::RGBHex(0x7fffd4) },
		{ "azure", Color::RGBHex(0xf0ffff) },
		{ "beige", Color::RGBHex(0xf5f5dc) },
		{ "bisque", Color::RGBHex(0xffe4c4) },
		{ "blanchedalmond", Color::RGBHex(0xffebcd) },
		{ "blueviolet", Color::RGBHex(0x8a2be2) },
		{ "brown", Color::RGBHex(0xa52a2a) },
		{ "burlywood", Color::RGBHex(0xdeb887) },
		{ "cadetblue", Color::RGBHex(0x5f9ea0) },
		{ "chartreuse", Color::RGBHex(0x7fff00) },
		{ "chocolate", Color::RGBHex(0xd2691e) },
		{ "coral", Color::RGBHex(0xff7f50) },
		{ "cornflowerblue", Color::RGBHex(0x6495ed) },
		{ "cornsilk", Color::RGBHex(0xfff8dc) },
		{ "crimson", Color::RGBHex(0xdc143c) },
		{ "cyan", Color::RGBHex(0x00ffff) },
		{ "aqua", Color::RGBHex(0x00ffff) },
		{ "darkblue", Color::RGBHex(0x00008b) },
		{ "darkcyan", Color::RGBHex(0x008b8b) },
		{ "darkgoldenrod", Color::RGBHex(0xb8860b) },
		{ "darkgray", Color::RGBHex(0xa9a9a9) },
		{ "darkgreen", Color::RGBHex(0x006400) },
		{ "darkgrey", Color::RGBHex(0xa9a9a9) },
		{ "darkkhaki", Color::RGBHex(0xbdb76b) },
		{ "darkmagenta", Color::RGBHex(0x8b008b) },
		{ "darkolivegreen", Color::RGBHex(0x556b2f) },
		{ "darkorange", Color::RGBHex(0xff8c00) },
		{ "darkorchid", Color::RGBHex(0x9932cc) },
		{ "darkred", Color::RGBHex(0x8b0000) },
		{ "darksalmon", Color::RGBHex(0xe9967a) },
		{ "darkseagreen", Color::RGBHex(0x8fbc8f) },
		{ "darkslateblue", Color::RGBHex(0x483d8b) },
		{ "darkslategray", Color::RGBHex(0x2f4f4f) },
		{ "darkslategrey", Color::RGBHex(0x2f4f4f) },
		{ "darkturquoise", Color::RGBHex(0x00ced1) },
		{ "darkviolet", Color::RGBHex(0x9400d3) },
		{ "deeppink", Color::RGBHex(0xff1493) },
		{ "deepskyblue", Color::RGBHex(0x00bfff) },
		{ "dimgray", Color::RGBHex(0x696969) },
		{ "dimgrey", Color::RGBHex(0x696969) },
		{ "dodgerblue", Color::RGBHex(0x1e90ff) },
		{ "firebrick", Color::RGBHex(0xb22222) },
		{ "floralwhite", Color::RGBHex(0xfffaf0) },
		{ "forestgreen", Color::RGBHex(0x228b22) },
		{ "gainsboro", Color::RGBHex(0xdcdcdc) },
		{ "ghostwhite", Color::RGBHex(0xf8f8ff) },
		{ "gold", Color::RGBHex(0xffd700) },
		{ "goldenrod", Color::RGBHex(0xdaa520) },
		{ "greenyellow", Color::RGBHex(0xadff2f) },
		{ "grey", Color::RGBHex(0x808080) },
		{ "honeydew", Color::RGBHex(0xf0fff0) },
		{ "hotpink", Color::RGBHex(0xff69b4) },
		{ "indianred", Color::RGBHex(0xcd5c5c) },
		{ "indigo", Color::RGBHex(0x4b0082) },
		{ "ivory", Color::RGBHex(0xfffff0) },
		{ "khaki", Color::RGBHex(0xf0e68c) },
		{ "lavender", Color::RGBHex(0xe6e6fa) },
		{ "lavenderblush", Color::RGBHex(0xfff0f5) },
		{ "lawngreen", Color::RGBHex(0x7cfc00) },
		{ "lemonchiffon", Color::RGBHex(0xfffacd) },
		{ "lightblue", Color::RGBHex(0xadd8e6) },
		{ "lightcoral", Color::RGBHex(0xf08080) },
		{ "lightcyan", Color::RGBHex(0xe0ffff) },
		{ "lightgoldenrodyellow", Color::RGBHex(0xfafad2) },
		{ "lightgray", Color::RGBHex(0xd3d3d3) },
		{ "lightgreen", Color::RGBHex(0x90ee90) },
		{ "lightgrey", Color::RGBHex(0xd3d3d3) },
		{ "lightpink", Color::RGBHex(0xffb6c1) },
		{ "lightsalmon", Color::RGBHex(0xffa07a) },
		{ "lightseagreen", Color::RGBHex(0x20b2aa) },
		{ "lightskyblue", Color::RGBHex(0x87cefa) },
		{ "lightslategray", Color::RGBHex(0x778899) },
		{ "lightslategrey", Color::RGBHex(0x778899) },
		{ "lightsteelblue", Color::RGBHex(0xb0c4de) },
		{ "lightyellow", Color::RGBHex(0xffffe0) },
		{ "limegreen", Color::RGBHex(0x32cd32) },
		{ "linen", Color::RGBHex(0xfaf0e6) },
		{ "magenta", Color::RGBHex(0xff00ff) },
		{ "fuchsia", Color::RGBHex(0xff00ff) },
		{ "mediumaquamarine", Color::RGBHex(0x66cdaa) },
		{ "mediumblue", Color::RGBHex(0x0000cd) },
		{ "mediumorchid", Color::RGBHex(0xba55d3) },
		{ "mediumpurple", Color::RGBHex(0x9370db) },
		{ "mediumseagreen", Color::RGBHex(0x3cb371) },
		{ "mediumslateblue", Color::RGBHex(0x7b68ee) },
		{ "mediumspringgreen", Color::RGBHex(0x00fa9a) },
		{ "mediumturquoise", Color::RGBHex(0x48d1cc) },
		{ "mediumvioletred", Color::RGBHex(0xc71585) },
		{ "midnightblue", Color::RGBHex(0x191970) },
		{ "mintcream", Color::RGBHex(0xf5fffa) },
		{ "mistyrose", Color::RGBHex(0xffe4e1) },
		{ "moccasin", Color::RGBHex(0xffe4b5) },
		{ "navajowhite", Color::RGBHex(0xffdead) },
		{ "oldlace", Color::RGBHex(0xfdf5e6) },
		{ "olivedrab", Color::RGBHex(0x6b8e23) },
		{ "orangered", Color::RGBHex(0xff4500) },
		{ "orchid", Color::RGBHex(0xda70d6) },
		{ "palegoldenrod", Color::RGBHex(0xeee8aa) },
		{ "palegreen", Color::RGBHex(0x98fb98) },
		{ "paleturquoise", Color::RGBHex(0xafeeee) },
		{ "palevioletred", Color::RGBHex(0xdb7093) },
		{ "papayawhip", Color::RGBHex(0xffefd5) },
		{ "peachpuff", Color::RGBHex(0xffdab9) },
		{ "peru", Color::RGBHex(0xcd853f) },
		{ "pink", Color::RGBHex(0xffc0cb) },
		{ "plum", Color::RGBHex(0xdda0dd) },
		{ "powderblue", Color::RGBHex(0xb0e0e6) },
		{ "rosybrown", Color::RGBHex(0xbc8f8f) },
		{ "royalblue", Color::RGBHex(0x4169e1) },
		{ "saddlebrown", Color::RGBHex(0x8b4513) },
		{ "salmon", Color::RGBHex(0xfa8072) },
		{ "sandybrown", Color::RGBHex(0xf4a460) },
		{ "seagreen", Color::RGBHex(0x2e8b57) },
		{ "seashell", Color::RGBHex(0xfff5ee) },
		{ "sienna", Color::RGBHex(0xa0522d) },
		{ "skyblue", Color::RGBHex(0x87ceeb) },
		{ "slateblue", Color::RGBHex(0x6a5acd) },
		{ "slategray", Color::RGBHex(0x708090) },
		{ "slategrey", Color::RGBHex(0x708090) },
		{ "snow", Color::RGBHex(0xfffafa) },
		{ "springgreen", Color::RGBHex(0x00ff7f) },
		{ "steelblue", Color::RGBHex(0x4682b4) },
		{ "tan", Color::RGBHex(0xd2b48c) },
		{ "thistle", Color::RGBHex(0xd8bfd8) },
		{ "tomato", Color::RGBHex(0xff6347) },
		{ "turquoise", Color::RGBHex(0x40e0d0) },
		{ "violet", Color::RGBHex(0xee82ee) },
		{ "wheat", Color::RGBHex(0xf5deb3) },
		{ "whitesmoke", Color::RGBHex(0xf5f5f5) },
		{ "yellowgreen", Color::RGBHex(0x9acd32) },
		{ "rebeccapurple", Color::RGBHex(0x663399) },
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
