#pragma once

namespace CE::Widgets
{

    CLASS()
    class CRYSTALWIDGETS_API CImage : public CWidget
    {
        CE_CLASS(CImage, CWidget)
    public:

        CImage();

        virtual ~CImage();

        Vec2 CalculateIntrinsicSize(f32 width, f32 height) override;
        
    private:

        void Construct() override;

        void HandleEvent(CEvent* event) override;

        void OnPaint(CPaintEvent* paintEvent) override;

    };
    
} // namespace CE::Widgets

#include "CImage.rtti.h"