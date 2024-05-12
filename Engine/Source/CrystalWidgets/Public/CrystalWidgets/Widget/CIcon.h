#pragma once

namespace CE::Widgets
{

    CLASS()
    class CRYSTALWIDGETS_API CIcon : public CWidget
    {
        CE_CLASS(CIcon, CWidget)
    public:

        CIcon();

        virtual ~CIcon();

        Vec2 CalculateIntrinsicSize(f32 width, f32 height) override;

    private:

        void Construct() override;

        void OnPaint(CPaintEvent* paintEvent) override;

    };
    
} // namespace CE::Widgets

#include "CIcon.rtti.h"