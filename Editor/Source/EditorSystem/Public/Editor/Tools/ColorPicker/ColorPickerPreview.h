#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORSYSTEM_API ColorPickerPreview : public CWidget
    {
        CE_CLASS(ColorPickerPreview, CWidget)
    public:

        ColorPickerPreview();

        Vec2 CalculateIntrinsicSize(f32 width, f32 height) override;

        void SetColor(const Color& color);

    private:

        void Construct() override;

        void OnPaint(CPaintEvent* paintEvent) override;

        FIELD()
        Color color{};

    };
    
} // namespace CE::Editor

#include "ColorPickerPreview.rtti.h"