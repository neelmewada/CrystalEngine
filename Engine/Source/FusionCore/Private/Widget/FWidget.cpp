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

        if (event->type == FEventType::FocusChanged)
        {
            FFocusEvent* focusEvent = static_cast<FFocusEvent*>(event);

            if (isFocused != focusEvent->gotFocus)
            {
                isFocused = focusEvent->gotFocus;

                if (SupportsFocusEvents())
                {
                    if (isFocused)
                        OnGotFocus();
                    else
                        OnLostFocus();
                }
            }
        }

        if (event->IsMouseEvent() && (!event->isConsumed || event->consumer != this))
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

        // TODO: Implement matrix transformation support for input handling

        Rect rect = Rect::FromSize(rectPos, rectSize);

        return rect.Contains(localMousePos) ? this : nullptr;
    }

    bool FWidget::ParentExistsRecursive(FWidget* parent)
    {
        if (this->parent == nullptr)
            return false;
        if (this->parent == parent || this == parent)
            return true;

        return this->parent->ParentExistsRecursive(parent);
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

    void FWidget::Focus()
    {
        FFusionContext* context = GetContext();
        if (context)
        {
            context->SetFocusWidget(this);
        }
    }

    void FWidget::Unfocus()
    {
        FFusionContext* context = GetContext();
        if (IsFocused() && context)
        {
            context->SetFocusWidget(parent);
        }
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

    bool FWidget::AddChild(FWidget* child)
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

            return true;
        }

        return false;
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

    FWidget& FWidget::Width(f32 width)
    {
        return (*this)
            .MinWidth(width)
            .MaxWidth(width);
    }

    FWidget& FWidget::Height(f32 height)
    {
        return (*this)
            .MinHeight(height)
            .MaxHeight(height);
    }

    FWidget& FWidget::Style(FStyle* style)
    {
        if (style && IsOfType(style->GetWidgetClass()))
        {
            m_Style = style;
            m_Style->MakeStyle(*this);
            MarkDirty();
        }
        return *this;
    }

    FWidget& FWidget::Style(const CE::Name& styleKey)
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

    FWidget& FWidget::Style(FStyleSet* styleSet, const CE::Name& styleKey)
    {
        if (!styleSet)
            return *this;

        FStyle* style = styleSet->FindStyle(styleKey);
        if (!style)
            return *this;

        return Style(style);
    }

} // namespace CE

