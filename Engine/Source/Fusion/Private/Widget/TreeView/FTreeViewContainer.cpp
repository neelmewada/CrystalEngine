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

        OnModelUpdate();
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

        // Vec2 transformedMousePos = (Matrix4x4::Translation(computedSize * m_Anchor) *
        //     Matrix4x4::Angle(-m_Angle) *
        //     Matrix4x4::Scale(invScale) *
        //     Matrix4x4::Translation(-computedPosition - m_Translation - computedSize * m_Anchor)) *
        //     Vec4(localMousePos.x, localMousePos.y, 0, 1);
        Vec2 transformedMousePos = mouseTransform * Vec4(localMousePos.x, localMousePos.y, 0, 1);

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

        if (event->IsMouseEvent() && event->sender == this)
        {
            FMouseEvent* mouseEvent = static_cast<FMouseEvent*>(event);

            if (mouseEvent->type == FEventType::MousePress && mouseEvent->IsLeftButton())
            {
                treeView->SelectionModel()->ClearSelection();
                treeView->ApplyStyle();
            }
        }

        Super::HandleEvent(event);
    }

    void FTreeViewContainer::OnModelUpdate()
    {
        if (treeView == nullptr || treeView->m_Model == nullptr || !treeView->m_Model->IsReady())
            return;
        if (!treeView->m_GenerateRowDelegate.IsValid())
            return;

        auto model = treeView->m_Model;

        int childIndex = 0;
        f32 curPosY = 0;
        f32 scrollY = -Translation().y;
        int globalRowIdx = 0;

        if (treeView->header)
        {
            model->SetHeaderData(*treeView->header);
        }

        Delegate<void(const FModelIndex&, int)> visitor = [&](const FModelIndex& parent, int indentLevel) -> void
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
                        globalRowIdx++;
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
                        rowWidget = &treeView->m_GenerateRowDelegate();
                        children.Insert(rowWidget);
                    }

                    rowWidget->SetParent(this);
                    rowWidget->index = index;
                    rowWidget->Enabled(true);
                    rowWidget->isAlternate = (globalRowIdx % 2 != 0);
                    rowWidget->treeView = treeView;
                    rowWidget->isHovered = false;

                    auto ctx = GetContext();
                    rowWidget->SetContextRecursively(ctx);
                    rowWidget->ApplyStyleRecursively();

                    childIndex++;
                    curPosY += rowHeight;

                    model->SetData(i, *rowWidget, parent);

                    int childrenCount = model->GetRowCount(index);

                    for (int c = 0; c < treeView->header->GetColumnCount() && c < rowWidget->GetCellCount(); ++c)
                    {
                        f32 minWidth = treeView->header->GetColumn(c)->GetComputedSize().x;
                        rowWidget->Visible(true);

                        FTreeViewCell& cell = *rowWidget->GetCell(c);

                        cell
							.ArrowVisible(treeView->m_ExpandableColumn == c && childrenCount > 0)
							.ArrowEnabled(treeView->m_ExpandableColumn == c)
							.ArrowExpanded(treeView->m_SelectionModel->IsSelected(index))
                        ;

                        if (treeView->m_ExpandableColumn == c && indentLevel > 0)
                        {
                            f32 indentOffset = treeView->m_Indentation * indentLevel;
                            if (indentOffset < minWidth)
                            {
                                cell.Margin(Vec4(indentOffset, 0, 0, 0));
                                cell.Width(minWidth - indentOffset);
                            }
                            else
                            {
                                rowWidget->Visible(false);
                            }
                        }
                        else
                        {
                            cell.Width(minWidth);
                        }
                    }

                    globalRowIdx++;

                    if (childrenCount > 0 && expandedRows.Exists(index))
                    {
                        visitor(index, indentLevel + 1);
                    }
                }
            };

        visitor(FModelIndex(), 0);

        while (childIndex < children.GetCount())
        {
            children[childIndex]->Enabled(false);

            childIndex++;
        }

        treeView->ApplyStyle();
        MarkDirty();
    }

    void FTreeViewContainer::CalculateIntrinsicSize()
    {
        ZoneScoped;

        if (treeView == nullptr || treeView->m_Model == nullptr || !treeView->m_Model->IsReady())
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
            contentSize.height = totalRowHeight;
        }

        for (int i = 0; i < children.GetCount(); ++i)
        {
            if (!children[i]->Enabled())
                continue;

            children[i]->CalculateIntrinsicSize();
            contentSize.width = Math::Max(contentSize.width, children[i]->GetIntrinsicSize().width);

            if (!treeView->AutoHeight())
            {
                contentSize.height += children[i]->GetIntrinsicSize().height;
            }
        }

        intrinsicSize.width += contentSize.width;
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
            auto child = children[i];

	        if (!child->Enabled())
                continue;

            f32 rowHeight = 0;
            if (!treeView->m_RowHeightDelegate.IsValid())
            {
                rowHeight = treeView->m_RowHeight;
            }
            else
            {
                rowHeight = treeView->m_RowHeightDelegate(child->index);
            }

            child->SetComputedPosition(curPos);
            child->SetComputedSize(Vec2(availableSize.x, rowHeight));

        	child->PlaceSubWidgets();

            curPos.y += rowHeight;
        }

        // TODO: Place sub-widgets
    }
    
}

