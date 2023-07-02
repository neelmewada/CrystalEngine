#pragma once

namespace CE::Widgets
{
	struct CSSToken;
	class CSSStyleSheet;
    
    class COREWIDGETS_API CSSParser
    {
    public:
        
		CSSParser(Stream* stream);
		~CSSParser();

		static CSSStyleSheet* ParseStyleSheet(const String& css, CSSStyleSheet* parent = nullptr);
        
		inline const Array<CSSParserToken>& GetTokens() const { return tokens; }

		CSSStyleSheet* ParseStyleSheet(CSSStyleSheet* parent = nullptr);

    private:

		Array<CSSParserToken> tokens{};
    };

} // namespace CE::Widgets

