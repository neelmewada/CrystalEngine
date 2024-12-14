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

        bool SupportsDragEvents() const override { return true; }



    protected: // - Internal -

        void OnPaint(FPainter* painter) override;

        void HandleEvent(FEvent* event) override;

        void OnPositionEdited();

        bool isDragged = false;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(Vec2, NormalizedColorPosition);

        FUSION_EVENT(ScriptEvent<void(Color)>, OnColorEdited);
        FUSION_EVENT(ScriptEvent<void(f32 h, f32 s, f32 v)>, OnHSVColorEdited);

        FUSION_WIDGET;
    };

}

#include "FColorPicker.rtti.h"
