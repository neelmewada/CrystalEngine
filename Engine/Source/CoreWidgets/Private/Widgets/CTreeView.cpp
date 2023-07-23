#include "CoreWidgets.h"

namespace CE::Widgets
{

	CTreeItemView::CTreeItemView()
	{

	}

	CTreeItemView::~CTreeItemView()
	{

	}

	void CTreeItemView::UpdateLayoutIfNeeded()
	{
		if (!NeedsLayout())
			return;

		SetNeedsStyle(); // Force update style
		UpdateStyleIfNeeded();

		Vec2 size = CalculateIntrinsicContentSize(YGUndefined, YGUndefined);

		{
			auto leftMargin = YGNodeStyleGetMargin(node, YGEdgeLeft);

			if (leftMargin.unit == YGUnitUndefined || leftMargin.unit == YGUnitAuto)
				YGNodeStyleSetMargin(node, YGEdgeLeft, 20 + indent * 15);
			else if (leftMargin.unit == YGUnitPoint)
				YGNodeStyleSetMargin(node, YGEdgeLeft, 20 + indent * 15 + leftMargin.value);
		}

		{
			auto minWidth = YGNodeStyleGetMinWidth(node);

			if (minWidth.unit == YGUnitUndefined || minWidth.unit == YGUnitAuto)
				YGNodeStyleSetMinWidth(node, size.x);
			else if (minWidth.unit == YGUnitPoint)
				YGNodeStyleSetMinWidth(node, Math::Max(size.x, minWidth.value));
		}

		if (GetOwner() != nullptr)
		{
			auto parentSize = GetOwner()->GetComputedLayoutSize();
			size.x = parentSize.x;
		}

		if (size.x <= 0) size.x = YGUndefined;
		if (size.y <= 0) size.y = YGUndefined;

		YGNodeCalculateLayout(node, size.x, size.y, YGDirectionLTR);
		needsLayout = false;
	}

	Vec2 CTreeItemView::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return GUI::CalculateTextSize(label) + Vec2(10, 5);
	}

	void CTreeItemView::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();
		auto padding = GetComputedLayoutPadding();

		//if (!isLeaf)
		//	rect.x += 20;

		GUI::Text(rect + Rect(padding.left, 0, -padding.right, 0), label, defaultStyleState);
	}

    CTreeView::CTreeView()
    {

    }

    CTreeView::~CTreeView()
    {
        
    }

	Vec2 CTreeView::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(100, calculatedHeight);
	}

    void CTreeView::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();

		DrawDefaultBackground();

		GUI::TreeViewFlags treeFlags = GUI::TVF_None;
		if (AlwaysShowVerticalScroll())
			treeFlags |= GUI::TVF_AlwaysShowVerticalScroll;

		calculatedHeight = 0;

		if (GUI::BeginTreeView(rect, GetUuid(), GetName().GetString(), treeFlags))
		{
			if (model != nullptr)
				DrawChildren({});

			GUI::EndTreeView();
			PollEvents();
		}

    }

	void CTreeView::DrawChildren(const CModelIndex& parent, int indent)
	{
		if (model == nullptr)
			return;

		int children = model->GetRowCount(parent);
        
		for (int i = 0; i < children; i++)
		{
			CModelIndex index = model->GetIndex(i, 0, parent);
			ClassType* widgetClass = model->GetWidgetClass(model->GetIndex(i, 0, parent));
			if (widgetClass == nullptr)
				continue;

			CTreeItemView* widget = nullptr;
			if (usedWidgetMap.KeyExists(index))
			{
				widget = usedWidgetMap[index];
			}
			else if (freeWidgetMap.KeyExists(widgetClass->GetTypeId()) && freeWidgetMap[widgetClass->GetTypeId()].NonEmpty())
			{
				widget = freeWidgetMap[widgetClass->GetTypeId()].Top();
				freeWidgetMap[widgetClass->GetTypeId()].Pop();
				usedWidgetMap[index] = widget;
				widget->SetNeedsStyle();
				widget->SetNeedsLayout();
			}
			else
			{
				widget = CreateWidget<CTreeItemView>(this, "TreeItemView", widgetClass);
				usedWidgetMap[index] = widget;
			}

			widget->indent = indent;
			model->SetData(index, widget);

			widget->nodeId = GetHash<CModelIndex>(index);

			if (!itemColorFetched || widget->NeedsStyle())
			{
				itemHovered = widget->defaultStyleState;
				itemActive = itemHovered;

				auto hoveredStyle = widget->stylesheet->SelectStyle(widget, CStateFlag::Hovered);
				for (const auto& [property, styleValue] : hoveredStyle.properties)
				{
					LoadGuiStyleStateProperty(property, styleValue, itemHovered);
				}

				auto activeStyle = widget->stylesheet->SelectStyle(widget, CStateFlag::Active);
				for (const auto& [property, styleValue] : activeStyle.properties)
				{
					LoadGuiStyleStateProperty(property, styleValue, itemActive);
				}

				itemColorFetched = true;
			}
			
			widget->UpdateStyleIfNeeded();
			widget->UpdateLayoutIfNeeded();
			bool isLeaf = model->GetRowCount(index) == 0;

			auto size = widget->GetComputedLayoutSize();
			auto padding = widget->GetComputedLayoutPadding();
			calculatedHeight += size.y;

			GUI::TreeNodeFlags flags = GUI::TNF_None;
			if (isLeaf)
				flags |= GUI::TNF_Leaf;

			auto cursorPos = GUI::GetCursorPos();

			bool selected = (selectedIndex == index);

			widget->DrawBackground((selected || widget->isLeftMousePressedInside) ? itemActive : (widget->isHovered ? itemHovered : widget->defaultStyleState), 
				Rect(cursorPos, cursorPos + size + Vec2(indent * 15.0f, 0)));

			bool isOpen = false;

			if (!selectableItems)
				isOpen = GUI::TreeViewNode(size, widget->nodeId, indent * 15.0f, padding, flags);
			else
				isOpen = GUI::TreeViewNodeSelectable(size, widget->nodeId, indent * 15.0f, &selected, &widget->isHovered, &widget->isLeftMousePressedInside, padding, flags);

			if (isOpen != widget->isOpen)
			{
				widget->isOpen = isOpen;
				widget->SetNeedsStyle();
				widget->SetNeedsLayout();
			}

            if (selected && selectedIndex != index)
            {
				selectedIndex = index;
				model->OnIndexSelected(index);
            }

			GUI::SetCursorPos(cursorPos);
            
			GUI::PushChildCoordinateSpace(Rect(cursorPos, cursorPos + size));

			widget->Render();

			GUI::PopChildCoordinateSpace();

			GUI::SetCursorPos(cursorPos + Vec2(0, size.y));

			if (isOpen)
			{
				if (model->GetRowCount(index) > 0)
					DrawChildren(index, indent + 1);

				GUI::TreeViewNodePop();
			}
			else
			{
				// Closed
			}
		}
	}

} // namespace CE

