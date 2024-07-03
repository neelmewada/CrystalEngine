#include "FusionCore.h"

namespace CE
{

    FStyledWidget::FStyledWidget()
    {
        m_BackgroundShape = FRectangle();
        m_Opacity = 1.0f;
    }

    void FStyledWidget::CalculateIntrinsicSize()
    {
	    Super::CalculateIntrinsicSize();
    }

    void FStyledWidget::OnPaint(FPainter* painter)
    {
        if (m_Opacity < 0.0001f)
            return;

        bool isButton = IsOfType<FButton>();

        if (m_Opacity < 1.0f)
        {
	        painter->PushOpacity(m_Opacity);
        }

        if (m_ClipShape.GetShapeType() != FShapeType::None)
        {
            painter->PushClipShape(localTransform, computedSize, m_ClipShape);
        }

        bool transformChanged = false;

        if (m_Translation.GetSqrMagnitude() > 0 || abs(m_Angle) > 0 || m_Scale.GetSqrMagnitude() != 2)
        {
            painter->SetItemTransform(Matrix4x4::Translation(m_Translation) * Matrix4x4::Angle(m_Angle) * Matrix4x4::Scale(Vec3(m_Scale.x, m_Scale.y, 1)));
            transformChanged = true;
        }

	    if (m_BackgroundShape.GetShapeType() != FShapeType::None && m_Background.GetBrushStyle() != FBrushStyle::None)
	    {
            painter->SetBrush(m_Background);
            if (m_BorderWidth > 0 && m_BorderColor.a > 0)
            {
                painter->SetPen(FPen(m_BorderColor, m_BorderWidth));
            }
            else
            {
                painter->SetPen(FPen());
            }

            switch (m_Background.GetBrushStyle())
            {
            case FBrushStyle::None:
                break;
            case FBrushStyle::SolidFill:
                break;
            case FBrushStyle::Texture:
                break;
            case FBrushStyle::LinearGradient:
                break;
            }

            painter->DrawShape(Rect::FromSize(computedPosition, computedSize), m_BackgroundShape);
	    }

        if (transformChanged)
        {
	        painter->SetItemTransform(Matrix4x4::Identity());
        }

        OnPaintContent(painter);

        // Paint child widgets
	    Super::OnPaint(painter);

        OnPaintContentOverlay(painter);

        if (m_ClipShape.GetShapeType() != FShapeType::None)
        {
            painter->PopClipShape();
        }

        if (m_Opacity < 1.0f)
        {
	        painter->PopOpacity();
        }
    }

    void FStyledWidget::SetContextRecursively(FFusionContext* context)
    {
	    Super::SetContextRecursively(context);

        ApplyStyle();
    }

    void FStyledWidget::OnAttachedToParent(FWidget* parent)
    {
	    Super::OnAttachedToParent(parent);

    }

}

