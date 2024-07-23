#include "FusionCore.h"

namespace CE
{

    FContainerWidget::FContainerWidget()
    {
        m_ClipChildren = false;
    }

    void FContainerWidget::SetContextRecursively(FFusionContext* context)
    {
	    Super::SetContextRecursively(context);

        for (FWidget* child : children)
        {
            child->SetContextRecursively(context);
        }
    }

    FWidget* FContainerWidget::HitTest(Vec2 localMousePos)
    {
	    FWidget* thisHitTest = Super::HitTest(localMousePos);
        if (thisHitTest == nullptr)
            return nullptr;
        if (children.IsEmpty())
            return thisHitTest;

        Vec2 invScale = Vec2(1 / m_Scale.x, 1 / m_Scale.y);

        Vec2 transformedMousePos = (Matrix4x4::Translation(-computedPosition - m_Translation) *
            Matrix4x4::Angle(-m_Angle) *
            Matrix4x4::Scale(invScale)) *
            Vec4(localMousePos.x, localMousePos.y, 0, 1);
        
	    for (int i = children.GetSize() - 1; i >= 0; --i)
	    {
            FWidget* child = children[i];
            if (!child->Enabled())
                continue;

            FWidget* result = child->HitTest(transformedMousePos);
            if (result)
            {
                return result;
            }
	    }

        return thisHitTest;
    }

    bool FContainerWidget::ChildExistsRecursive(FWidget* child)
    {
        if (this == child)
            return true;

        for (FWidget* widget : children)
        {
	        if (widget->ChildExistsRecursive(child))
                return true;
        }

        return false;
    }

    void FContainerWidget::HandleEvent(FEvent* event)
    {
        if (event->stopPropagation)
            return;

        if (event->direction == FEventDirection::TopToBottom)
        {
            for (FWidget* child : children)
            {
                if (!child->Enabled())
                    continue;

                child->HandleEvent(event);

                if (event->stopPropagation)
                {
                    event->stopPropagation = false;
                }
            }
        }

        Super::HandleEvent(event);
    }

    void FContainerWidget::InsertChild(int index, FWidget* child)
    {
        if (AddChild(child))
        {
            children.Remove(child);
            children.InsertAt(index, child);
        }
    }

    void FContainerWidget::DestroyAllChildren()
    {
	    for (int i = children.GetSize() - 1; i >= 0; --i)
	    {
            FWidget* child = children[i];
            RemoveChild(child);
            child->Destroy();
	    }
    }

    void FContainerWidget::RemoveAllChildren()
    {
        for (int i = children.GetSize() - 1; i >= 0; --i)
        {
            FWidget* child = children[i];
            RemoveChild(child);
        }
    }

    void FContainerWidget::ClearStyle()
    {
	    Super::ClearStyle();

        for (FWidget* child : children)
        {
            child->ClearStyle();
        }
    }

    bool FContainerWidget::TryAddChild(FWidget* child)
    {
        if (children.Exists(child))
            return false;

        children.Add(child);

        MarkLayoutDirty();
        return true;
    }

    bool FContainerWidget::TryRemoveChild(FWidget* child)
    {
        if (!child)
            return false;
        
        int index = children.IndexOf(child);
        if (index < 0)
            return false;

        children.RemoveAt(index);

        MarkLayoutDirty();
    	return true;
    }

    void FContainerWidget::OnChildWidgetDestroyed(FWidget* child)
    {
	    Super::OnChildWidgetDestroyed(child);
        
        TryRemoveChild(child);
    }

} // namespace CE
