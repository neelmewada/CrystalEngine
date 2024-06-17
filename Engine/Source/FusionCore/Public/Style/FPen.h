#pragma once

namespace CE
{
    ENUM()
    enum class FPenStyle : u32
    {
	    None,
        SolidLine
    };
    ENUM_CLASS(FPenStyle);

    STRUCT()
    struct FUSIONCORE_API FPen
    {
        CE_STRUCT(FPen)
    public:

        FPen();
        virtual ~FPen();

        FPen(const Color& penColor, f32 thickness = 1.0f, FPenStyle penStyle = FPenStyle::SolidLine);

        const Color& GetColor() const { return color; }
        void SetColor(const Color& penColor) { this->color = penColor; }

        f32 GetThickness() const { return thickness; }
        void SetThickness(f32 thickness) { this->thickness = thickness; }

        FPenStyle GetStyle() const { return style; }
        void SetStyle(FPenStyle penStyle) { this->style = penStyle; }

    private:

        Color color;
        f32 thickness = 0.0f;
        FPenStyle style = FPenStyle::None;

    };
    
} // namespace CE

#include "FPen.rtti.h"