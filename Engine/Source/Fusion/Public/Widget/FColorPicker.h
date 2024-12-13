#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FColorPicker : public FWidget
    {
        CE_CLASS(FColorPicker, FWidget)
    protected:

        FColorPicker();

        void Construct() override;

    public: // - Public API -

        bool SupportsMouseEvents() const override { return true; }

    protected: // - Internal -

        void OnPaint(FPainter* painter) override;

        void HandleEvent(FEvent* event) override;

        bool isMouseDown = false;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(Vec2, NormalizedColorPosition);

        FUSION_WIDGET;
    };

}

#include "FColorPicker.rtti.h"
