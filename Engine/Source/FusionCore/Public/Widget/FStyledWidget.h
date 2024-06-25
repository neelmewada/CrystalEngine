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

        void CalculateIntrinsicSize() override;

        // - Events -

        void OnPaint(FPainter* painter) override;

    protected:

        virtual void OnPaintContent(FPainter* painter) {}

        virtual void OnPaintContentOverlay(FPainter* painter) {}

        void SetContextRecursively(FFusionContext* context) override;

        void OnAttachedToParent(FWidget* parent) override;

    protected: // - Fusion Fields -

        FIELD()
        FBrush m_Background;

        FIELD()
        FShape m_BackgroundShape = FRectangle();

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

        Self& Border(const Color& borderColor, f32 borderWidth = 1.0f)
        {
            return (*this)
        		.BorderColor(borderColor)
				.BorderWidth(borderWidth);
        }

        FUSION_PROPERTY(Opacity);

        FUSION_FRIENDS;
        FUSION_WIDGET;
    };
    
}

#include "FStyledWidget.rtti.h"
