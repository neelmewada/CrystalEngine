#pragma once

namespace CE::Widgets
{
    enum class CPenStyle
    {
	    SolidLine = 0,
        DashedLine,
        DottedLine
    };
    ENUM_CLASS(CPenStyle);

    struct CPen
    {
    public:

        CPen() = default;

        CPen(const Color& color) : color(color)
        {}

        void SetColor(const Color& color)
        {
            this->color = color;
        }

        void SetWidth(f32 width)
        {
            this->width = width;
        }

        void SetStyle(CPenStyle style)
        {
            this->style = style;

            if (style == CPenStyle::DottedLine)
            {
                dashLength = 1.0f;
            }
        }

        CPenStyle GetStyle() const
        {
	        return style;
        }

        const Color& GetColor() const
        {
            return color;
        }

        f32 GetWidth() const
        {
            return width;
        }

        void SetDashLength(f32 dashLength)
        {
            if (style == CPenStyle::DottedLine)
            {
                dashLength = 1.0f;
            }

            this->dashLength = dashLength;
        }

        f32 GetDashLength() const
        {
            if (style == CPenStyle::DottedLine)
            {
                return 1.0f;
            }

            return dashLength;
        }

    private:

        Color color{};
        f32 width = 0.0f;
        CPenStyle style{};
        f32 dashLength = 4.0f;

        friend class CPainter;
    };
    
    
} // namespace CE::Widgets
