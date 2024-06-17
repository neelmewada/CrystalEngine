#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FStyledWidget : public FCompoundWidget
    {
        CE_CLASS(FStyledWidget, FCompoundWidget)
    public:

        FStyledWidget();

        // - Public API -


        // - Events -

        void OnPaint(FPainter* painter) override;

    protected: // - Fusion Fields -

        FIELD()
        FStyle* m_Style = nullptr;

        FIELD()
        FBrush m_Background;

        FIELD()
        FShape m_BackgroundShape;

        FIELD()
        FShape m_ClipShape;

        FIELD()
        Color m_BorderColor;

        FIELD()
        f32 m_BorderWidth = 0.0f;

        FIELD()
        f32 m_Opacity = 1.0f;

    public:   // - Fusion Properties -

        FUSION_PROPERTY(Background);
        FUSION_PROPERTY(BackgroundShape);
        FUSION_PROPERTY(ClipShape);

        Self& CornerRadius(const Vec4& cornerRadius)
        {
            return ClipShape(FShape::RoundedRect(cornerRadius));
        }

        Self& CornerRadius(f32 cornerRadius)
        {
            return ClipShape(FRoundedRectangle(cornerRadius));
        }

        FUSION_PROPERTY(BorderColor);

        FUSION_PROPERTY(BorderWidth);

        FUSION_PROPERTY(Opacity);

        Self& Style(FStyle* style);

        Self& Style(const CE::Name& styleKey);

        FUSION_FRIENDS;
        FUSION_WIDGET;
    };
    
}

#include "FStyledWidget.rtti.h"
