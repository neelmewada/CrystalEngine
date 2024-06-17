#include "FusionCore.h"

namespace CE
{

    FStyledWidget::FStyledWidget()
    {

    }

    void FStyledWidget::OnPaint(FPainter* painter)
    {
        if (m_Opacity < 0.0001f)
            return;

        if (m_Opacity < 1.0f)
        {
	        painter->PushOpacity(m_Opacity);
        }

        if (m_ClipShape.GetShapeType() != FShapeType::None)
        {
            painter->PushClipShape(localTransform, computedSize, m_ClipShape);
        }

	    if (m_BackgroundShape.GetShapeType() != FShapeType::None)
	    {
            painter->SetBrush(m_Background);
            if (m_BorderWidth > 0 && m_BorderColor.a > 0)
            {
                painter->SetPen(FPen(m_BorderColor, m_BorderWidth));
            }

            switch (m_Background.GetBrushStyle())
            {
            case FBrushStyle::None:
                break;
            case FBrushStyle::SolidFill:
                painter->DrawShape(Rect::FromSize(computedPosition, computedSize), m_BackgroundShape);
                break;
            case FBrushStyle::TexturePattern:
                break;
            case FBrushStyle::LinearGradient:
                break;
            }
	    }

        // Paint children
	    Super::OnPaint(painter);

        if (m_ClipShape.GetShapeType() != FShapeType::None)
        {
            painter->PopClipShape();
        }

        if (m_Opacity < 1.0f)
        {
	        painter->PopOpacity();
        }
    }

    FStyledWidget::Self& FStyledWidget::Style(FStyle* style)
    {
        if (style && IsOfType(style->GetWidgetClass()))
        {
            m_Style = style;
            m_Style->MakeStyle(*this);
            MarkDirty();
        }
        return *this;
    }

    FStyledWidget::Self& FStyledWidget::Style(const CE::Name& styleKey)
    {
        if (!styleKey.IsValid())
            return *this;

        FFusionContext* context = GetContext();
        if (!context)
            return *this;

        FStyleManager* styleManager = context->GetStyleManager();
        if (!styleManager)
            return *this;

        FStyle* style = styleManager->FindStyle(styleKey);
        return Style(style);
    }

}

