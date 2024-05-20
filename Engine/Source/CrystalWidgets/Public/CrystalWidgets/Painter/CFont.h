#pragma once

namespace CE::Widgets
{

    struct CFont
    {
    public:

        CFont()
        {}

        CFont(const Name& family, f32 size = 14, bool bold = false)
    	: family(family), size(size), bold(bold)
        {}

        void SetFamily(const Name& family) { this->family = family; }

        void SetSize(f32 size) { this->size = size; }

    	void SetBold(bool set) { this->bold = set; }

        Name GetFamily() const { return family; }

        u32 GetSize() const { return size; }

        bool IsBold() const { return bold; }

        void SetUnderline(bool underline) { this->underline = underline; }

        bool IsUnderline() const { return underline; }

        void SetLineStyle(CPenStyle style) { lineStyle = style; }

        CPenStyle GetLineStyle() const { return lineStyle; }

        void SetLineColor(const Color& color) { lineColor = color; }

        const Color& GetLineColor() const { return lineColor; }

    private:

        Name family = "";
        f32 size = 14;
        bool bold = false;
        bool underline = false;

        CPenStyle lineStyle = CPenStyle::SolidLine;
        Color lineColor{};

        friend class CPainter;
    };
    
} // namespace CE::Widgets
