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

		static CSSStyleSheet* ParseStyleSheet(const String& css, Object* owner = nullptr);

		static CSSStyleSheet* ParseStyleSheet(CSSStyleSheet* original, const String& css);
        
		inline const Array<CSSParserToken>& GetTokens() const { return tokens; }

		CSSStyleSheet* ParseStyleSheetAlloc(Object* owner = nullptr);
		bool ParseStyleSheet(CSSStyleSheet* original);

    private:

		void ParsePropertyValue(int& cursor, CStylePropertyType property, CStyleValue& outValue);

		void ParseVector(int startCursor, int endCursor, Vec4& out);
		void ParseColorU32(int cursor, Color& out);

		Array<CSSParserToken> tokens{};
    };

} // namespace CE::Widgets

