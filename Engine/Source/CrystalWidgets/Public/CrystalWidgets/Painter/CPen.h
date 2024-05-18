#pragma once

namespace CE::Widgets
{
    enum class CPenStyle
    {
	    SolidLine = 0,
        DashLine
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
            this->dashLength = dashLength;
        }

        f32 GetDashLength() const
        {
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
