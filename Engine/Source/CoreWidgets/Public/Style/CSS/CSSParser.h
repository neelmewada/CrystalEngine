#pragma once

namespace CE::Widgets
{
	struct CSSToken;
	class CSSStyleSheet;

	namespace Internal
	{
		struct CSSNode
		{

			Array<CSSNode*> children{};
		};

		struct CSSRuleNode : CSSNode
		{

		};

		struct CSSPropertyValueNode : CSSNode
		{
			union {
				f32 single;
				Vec4 vector{};
				Color color;
			};

			b8 isSingle = false;
			b8 isVector = false;
			b8 isColor = false;
		};

		struct CSSPropertyNode : CSSNode
		{
			String property{};
			CSSPropertyValueNode value{};
		};
	}
    
    class COREWIDGETS_API CSSParser
    {
    public:
        
		CSSParser(Stream* stream);
		~CSSParser();

		static CSSStyleSheet* ParseStyleSheet(const String& css, CSSStyleSheet* parent = nullptr);
        
		inline const Array<CSSParserToken>& GetTokens() const { return tokens; }

		CSSStyleSheet* ParseStyleSheet(CSSStyleSheet* parent = nullptr);

    private:

		void ParsePropertyValue(int& cursor, CStylePropertyType property, CStyleValue& outValue);

		void ParseVector(int startCursor, int endCursor, Vec4& out);
		void ParseColorU32(int cursor, Color& out);

		Array<CSSParserToken> tokens{};
    };

} // namespace CE::Widgets

