#include "Fusion.h"

namespace CE
{

    FTreeViewContainer::FTreeViewContainer()
    {
        
    }

    void FTreeViewContainer::Construct()
    {
        Super::Construct();

        
    }

    void FTreeViewContainer::OnPaint(FPainter* painter)
    {
        Super::OnPaint(painter);

        if (children.IsEmpty() || !Enabled())
            return;

        painter->PushChildCoordinateSpace(localTransform);
        //if (m_ClipChildren)
        {
            //painter->PushClipShape(Matrix4x4::Identity(), computedSize);
        }

        for (FTreeViewRow* child : children)
        {
            if (!child->Enabled() || !child->Visible())
                continue;

            child->OnPaint(painter);
        }

        //if (m_ClipChildren)
        {
            //painter->PopClipShape();
        }
        painter->PopChildCoordinateSpace();
    }

    void FTreeViewContainer::OnPostComputeLayout()
    {
	    Super::OnPostComputeLayout();

    }

    void FTreeViewContainer::OnModelUpdate()
    {
        if (treeView == nullptr || treeView->m_Model == nullptr)
            return;
        if (!treeView->m_GenerateRowDelegate.IsValid())
            return;

        auto model = treeView->m_Model;

        int childIndex = 0;
        f32 curPosY = 0;
        f32 scrollY = -Translation().y;

        if (treeView->header)
        {
            model->SetHeaderData(*treeView->header);
        }

        Delegate<void(const FModelIndex&)> visitor = [&](const FModelIndex& parent) -> void
            {
                int rowCount = model->GetRowCount(parent);
	            if (rowCount == 0)
	                return;

                for (int i = 0; i < rowCount; ++i)
                {
                    FModelIndex index = model->GetIndex(i, 0, parent);
                    if (!index.IsValid())
                        continue;

                    f32 rowHeight = 0;

                    if (!treeView->m_RowHeightDelegate.IsValid())
                    {
                        rowHeight = treeView->m_RowHeight;
                    }
                    else
                    {
                        rowHeight = treeView->m_RowHeightDelegate(index);
                    }

                    f32 topY = curPosY;
                    f32 bottomY = curPosY + rowHeight;

                    if (bottomY < scrollY)
                    {
                        curPosY += rowHeight; // We are above the scroll view
	                    continue;
                    }
                    else if (topY > scrollY + computedSize.y)
                    {
	                    break; // We are below the scroll view
                    }

                    FTreeViewRow* rowWidget = nullptr;
                    if (childIndex < children.GetCount())
                    {
                        rowWidget = children[childIndex];
                    }
                    else
                    {
                        rowWidget = &treeView->m_GenerateRowDelegate(index);
                        children.Insert(rowWidget);
                    }

                    rowWidget->index = index;
                    rowWidget->Enabled(true);

                    childIndex++;
                    curPosY += rowHeight;

                    model->SetData(i, *rowWidget, parent);

                    if (expandedRows.Exists(index) && model->GetRowCount(index) > 0)
                    {
                        visitor(index);
                    }
                }
            };

        visitor(FModelIndex());

        while (childIndex < children.GetCount())
        {
            children[childIndex]->Enabled(false);

            childIndex++;
        }

        MarkDirty();
    }

    void FTreeViewContainer::SetContextRecursively(FFusionContext* context)
    {
	    Super::SetContextRecursively(context);

        for (FTreeViewRow* child : children)
        {
            child->SetContextRecursively(context);
        }
    }

    FWidget* FTreeViewContainer::HitTest(Vec2 localMousePos)
    {
        FWidget* thisHitTest = Super::HitTest(localMousePos);
        if (thisHitTest == nullptr)
            return nullptr;
        if (children.IsEmpty())
            return thisHitTest;

        Vec3 invScale = Vec3(1 / m_Scale.x, 1 / m_Scale.y, 1);

        Vec2 transformedMousePos = (Matrix4x4::Translation(-computedPosition - m_Translation) *
            Matrix4x4::Angle(-m_Angle) *
            Matrix4x4::Scale(invScale)) *
            Vec4(localMousePos.x, localMousePos.y, 0, 1);

        for (int i = children.GetCount() - 1; i >= 0; --i)
        {
            FTreeViewRow* child = children[i];
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

    bool FTreeViewContainer::ChildExistsRecursive(FWidget* child)
    {
        if (this == child)
            return true;

        for (FTreeViewRow* widget : children)
        {
            if (widget->ChildExistsRecursive(child))
                return true;
        }

        return false;
    }

    void FTreeViewContainer::ApplyStyleRecursively()
    {
        Super::ApplyStyleRecursively();

        for (FTreeViewRow* widget : children)
        {
            widget->ApplyStyleRecursively();
        }
    }

    void FTreeViewContainer::HandleEvent(FEvent* event)
    {
        if (event->stopPropagation)
            return;

        if (event->direction == FEventDirection::TopToBottom)
        {
            for (FTreeViewRow* child : children)
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

    void FTreeViewContainer::CalculateIntrinsicSize()
    {
        ZoneScoped;

        if (treeView == nullptr || treeView->m_Model == nullptr)
        {
            Super::CalculateIntrinsicSize();
            return;
        }

        intrinsicSize.width = m_Padding.left + m_Padding.right;
        intrinsicSize.height = m_Padding.top + m_Padding.bottom;

        Vec2 contentSize = Vec2();

        auto model = treeView->m_Model;

        f32 totalRowHeight = 0;

        Delegate<void(const FModelIndex&)> visitor = [&](const FModelIndex& parent) -> void
            {
                int rowCount = model->GetRowCount(parent);
                if (rowCount == 0)
                    return;

                for (int i = 0; i < rowCount; ++i)
                {
                    FModelIndex index = model->GetIndex(i, 0, parent);

                    if (!treeView->m_RowHeightDelegate.IsValid())
                    {
	                    totalRowHeight += treeView->m_RowHeight;
                    }
                    else
                    {
                        totalRowHeight += treeView->m_RowHeightDelegate(index);
                    }

                    if (expandedRows.Exists(index))
                    {
                        visitor(index);
                    }
                }
            };

        if (treeView->AutoHeight())
        {
            visitor(FModelIndex());
        }
        else
        {
            contentSize.height = m_MinHeight;
        }

        for (int i = 0; i < children.GetCount(); ++i)
        {
            if (!children[i]->Enabled())
                continue;

            children[i]->CalculateIntrinsicSize();
        }

        intrinsicSize.width += m_MinWidth;
        intrinsicSize.height += contentSize.height;

        ApplyIntrinsicSizeConstraints();
    }

    void FTreeViewContainer::PlaceSubWidgets()
    {
        ZoneScoped;

        Super::PlaceSubWidgets();

        if (children.IsEmpty())
        {
            return;
        }

        f32 scrollY = -Translation().y;

        Vec2 curPos = Vec2(m_Padding.left, m_Padding.top);
        f32 remainingSize = 0;
        Vec2 availableSize = computedSize - Vec2(m_Padding.left + m_Padding.right,
            m_Padding.top + m_Padding.bottom);

        for (int i = 0; i < children.GetCount(); ++i)
        {
	        if (!children[i]->Enabled())
                continue;

            f32 rowHeight = 0;
            if (!treeView->m_RowHeightDelegate.IsValid())
            {
                rowHeight = treeView->m_RowHeight;
            }
            else
            {
                rowHeight = treeView->m_RowHeightDelegate(children[i]->index);
            }

            children[i]->SetComputedPosition(curPos);
            children[i]->SetComputedSize(Vec2(availableSize.x, ));
        }

        // TODO: Place sub-widgets
    }
    
}

