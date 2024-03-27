#pragma once

namespace CE::Widgets
{
    CLASS()
    class CRYSTALWIDGETS_API CLabel : public CWidget
    {
        CE_CLASS(CLabel, CWidget)
    public:

        CLabel();
        virtual ~CLabel();

        Vec2 CalculateIntrinsicSize(f32 width, f32 height) override;

        void SetText(const String& text);
        const String& GetText() const;

    crystalwidgets_internal:

        void OnPaint(CPaintEvent* paintEvent) override;

        FIELD()
        String text = "";


    };
    
} // namespace CE::Widgets

#include "CLabel.rtti.h"