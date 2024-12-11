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

        void SetContextRecursively(FFusionContext* context) override;

        // - Events -

        void OnPaint(FPainter* painter) override;

    protected:

        virtual void OnPaintContent(FPainter* painter) {}

        virtual void OnPaintContentOverlay(FPainter* painter) {}

        void OnAttachedToParent(FWidget* parent) override;

    protected: // - Fusion Fields -

    public:   // - Fusion Properties -

        FUSION_PROPERTY(FBrush, Background);
        FUSION_PROPERTY(FShape, BackgroundShape);
        FUSION_PROPERTY(FShape, ClipShape);

        Self& CornerRadius(const Vec4& cornerRadius)
        {
            return BackgroundShape(FShape::RoundedRect(cornerRadius));
        }

        Self& CornerRadius(f32 cornerRadius)
        {
            return BackgroundShape(FRoundedRectangle(cornerRadius));
        }

        FUSION_PROPERTY(Color, BorderColor);
        FUSION_PROPERTY(f32, BorderWidth);
        FUSION_PROPERTY(FPenStyle, BorderStyle);

        Self& Border(const Color& borderColor, f32 borderWidth = 1.0f)
        {
            return (*this)
        		.BorderColor(borderColor)
				.BorderWidth(borderWidth);
        }

        FUSION_PROPERTY(f32, Opacity);


        FUSION_WIDGET;
    };
    
}

#include "FStyledWidget.rtti.h"
