#include "FusionCore.h"

namespace CE
{

    FWidget::FWidget()
    {
        
    }

    FWidget::~FWidget()
    {
        
    }

    FFusionContext* FWidget::GetContext()
    {
        if (!context && parent)
        {
            context = parent->GetContext();
        }
        return context;
    }

    void FWidget::OnPaint(FPainter* painter)
    {
        globalPosition = painter->GetTopCoordinateSpace() * Vec4(computedPosition.x, computedPosition.y, 0, 1);
    }

    void FWidget::HandleEvent(FEvent* event)
    {
        if (event->stopPropagation)
            return;

        m_OnEvent(event);

        if (event->IsMouseEvent() && (!event->consumed || event->consumer != this))
        {
            
        }

        if (parent != nullptr && event->direction == FEventDirection::BottomToTop)
        {
            parent->HandleEvent(event);
        }
    }

    FWidget* FWidget::HitTest(Vec2 localMousePos)
    {
        Vec2 rectPos = computedPosition;
        Vec2 rectSize = computedSize;

        /*if (m_Translation.GetSqrMagnitude() > 0 || abs(m_Angle) > 0 || abs(m_Scale.GetSqrMagnitude() - 2.0f) > 0.0f)
        {
            Vec2 invScale = Vec2(1 / m_Scale.x, 1 / m_Scale.y);

            localMousePos = Matrix4x4::Translation(rectSize / 2) *
                Matrix4x4::Translation(-m_Translation) *
                Matrix4x4::Angle(-m_Angle) *
                Matrix4x4::Scale(invScale) *
                Matrix4x4::Translation(-rectSize / 2) *
                Vec4(localMousePos.x, localMousePos.y, 0, 1);
        }*/

        Rect rect = Rect::FromSize(rectPos, rectSize);

        return rect.Contains(localMousePos) ? this : nullptr;
    }

    void FWidget::OnAttachedToParent(FWidget* parent)
    {
        ApplyStyle();
    }

    void FWidget::OnDetachedFromParent(FWidget* parent)
    {
    }

    void FWidget::ClearStyle()
    {
        m_Style = nullptr;
    }

    void FWidget::OnFusionPropertyModified(const CE::Name& propertyName)
    {
        static const HashSet<CE::Name> transformProperties = { "Translation", "Angle", "Scale" };

        if (transformProperties.Exists(propertyName))
        {
	        UpdateLocalTransform();
        }
    }

    void FWidget::OnAfterConstruct()
    {
	    Super::OnAfterConstruct();

        Construct();
    }

    void FWidget::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        if (GetContext())
        {
            GetContext()->OnWidgetDestroyed(this);
        }

        if (parent)
        {
            parent->OnChildWidgetDestroyed(this);
        }
        parent = nullptr;
    }

    void FWidget::CalculateIntrinsicSize()
    {
        intrinsicSize = Vec2(m_MinWidth + m_Padding.left + m_Padding.right, 
            m_MinHeight + m_Padding.top + m_Padding.bottom);

        ApplyIntrinsicSizeConstraints();
    }

    void FWidget::PlaceSubWidgets()
    {
        ApplySizeConstraints();

        UpdateLocalTransform();
    }

    void FWidget::ApplyIntrinsicSizeConstraints()
    {
        intrinsicSize.width = Math::Clamp(intrinsicSize.width,
            m_MinWidth + m_Padding.left + m_Padding.right,
            m_MaxWidth + m_Padding.left + m_Padding.right);

        intrinsicSize.height = Math::Clamp(intrinsicSize.height,
            m_MinHeight + m_Padding.top + m_Padding.bottom,
            m_MaxHeight + m_Padding.top + m_Padding.bottom);
    }

    void FWidget::ApplySizeConstraints()
    {
        computedSize.width = Math::Clamp(computedSize.width, 
            m_MinWidth + m_Padding.left + m_Padding.right,
            m_MaxWidth + m_Padding.left + m_Padding.right);

        computedSize.height = Math::Clamp(computedSize.height,
            m_MinHeight + m_Padding.bottom + m_Padding.top,
            m_MaxHeight + m_Padding.bottom + m_Padding.top);
    }

    void FWidget::UpdateLocalTransform()
    {
        localTransform =
            Matrix4x4::Translation(computedPosition + m_Translation) *
            Matrix4x4::Angle(m_Angle) *
            Matrix4x4::Scale(Vec3(m_Scale.x, m_Scale.y, 1));
    }

    void FWidget::AddChild(FWidget* child)
    {
        if (TryAddChild(child))
        {
            if (child->GetOuter() == nullptr)
            {
                AttachSubobject(child);
            }

            child->parent = this;
            child->OnAttachedToParent(this);
            MarkLayoutDirty();
        }
    }

    void FWidget::RemoveChild(FWidget* child)
    {
        if (TryRemoveChild(child))
        {
            child->OnDetachedFromParent(this);
            child->parent = nullptr;
            MarkLayoutDirty();
        }
    }

    void FWidget::ApplyStyle()
    {
        if (styleKey.IsValid() && m_Style == nullptr)
        {
            Style(styleKey);
        }
        else
        {
            Style(m_Style);
        }
    }

    void FWidget::SetContextRecursively(FFusionContext* context)
    {
        this->context = context;

        ApplyStyle();
    }

    void FWidget::MarkLayoutDirty()
    {
        FFusionContext* context = GetContext();
        if (context)
        {
            context->MarkLayoutDirty();
        }
    }

    void FWidget::MarkDirty()
    {
        FFusionContext* context = GetContext();
        if (context)
        {
            context->MarkDirty();
        }
    }

    Vec2 FWidget::GetGlobalPosition() const
    {
        return globalPosition;
    }

    void FWidget::Construct()
    {
        
    }

    FWidget::Self& FWidget::Style(FStyle* style)
    {
        if (style && IsOfType(style->GetWidgetClass()))
        {
            m_Style = style;
            m_Style->MakeStyle(*this);
            MarkDirty();
        }
        return *this;
    }

    FWidget::Self& FWidget::Style(const CE::Name& styleKey)
    {
        if (!styleKey.IsValid())
            return *this;

        this->styleKey = styleKey;

        FFusionContext* context = GetContext();
        if (!context)
            return *this;

        FStyleSet* defaultStyleSet = context->GetDefaultStyleSet();
        if (!defaultStyleSet)
            return *this;

        FStyle* style = defaultStyleSet->FindStyle(styleKey);
        return Style(style);
    }

    FWidget::Self& FWidget::Style(FStyleSet* styleSet, const CE::Name& styleKey)
    {
        if (!styleSet)
            return *this;

        FStyle* style = styleSet->FindStyle(styleKey);
        if (!style)
            return *this;

        return Style(style);
    }

} // namespace CE

