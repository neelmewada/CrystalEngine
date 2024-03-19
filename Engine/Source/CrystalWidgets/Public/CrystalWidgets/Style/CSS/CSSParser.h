#pragma once

namespace CE::Widgets
{

    class CRYSTALWIDGETS_API CSSParser
    {
    public:
        CSSParser(Stream* stream);
        virtual ~CSSParser();

        static CSSStyleSheet* ParseStyleSheet(const String& css, Object* owner = nullptr);

        static CSSStyleSheet* ParseStyleSheet(CSSStyleSheet* original, const String& css);

        bool ParseStyleSheet(CSSStyleSheet* original);

        CSSStyleSheet* ParseStyleSheet(Object* owner = nullptr);

        const String& GetErrorMessage() const { return errorMessage; }

    private:

        void ParsePropertyValue(int& cursor, CStylePropertyType property, CStyleValue& outValue);

        void ParseVector(int startCursor, int endCursor, Vec4& out);
        void ParseColorU32(int cursor, Color& out);

        String errorMessage{};

        Array<CSSParserToken> tokens{};
    };
    
} // namespace CE::Widgets
