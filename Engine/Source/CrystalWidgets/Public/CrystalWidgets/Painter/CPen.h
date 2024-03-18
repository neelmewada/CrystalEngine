#pragma once

namespace CE::Widgets
{

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

    private:

        Color color{};
        f32 width = 0.0f;

        friend class CPainter;
    };
    
    
} // namespace CE::Widgets
