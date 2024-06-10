#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FStyledWidget : public FCompoundWidget
    {
        CE_CLASS(FStyledWidget, FCompoundWidget)
    public:

        FStyledWidget();

    protected:

    protected: // - Fusion Fields -

        FIELD()
        FStyle* m_Style = nullptr;

        FIELD()
        FBrush m_Background;

        FIELD()
        Color m_ForegroundColor;

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

        FUSION_PROPERTY(ForegroundColor);

        FUSION_PROPERTY(ClipShape);

        FUSION_PROPERTY(BorderColor);

        FUSION_PROPERTY(BorderWidth);

        FUSION_PROPERTY(Opacity);

        Self& UseStyle(FStyle* style);

        Self& UseStyle(const CE::Name& styleKey);

        FUSION_FRIENDS;
        FUSION_WIDGET;
    };
    
}

#include "FStyledWidget.rtti.h"
