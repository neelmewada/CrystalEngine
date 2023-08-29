#include "CoreWidgets.h"

namespace CE::Widgets
{
	CSplitViewContainer::CSplitViewContainer()
	{
	}

	CSplitViewContainer::~CSplitViewContainer()
	{
	}

	void CSplitViewContainer::Construct()
	{
		Super::Construct();
	}

	void CSplitViewContainer::OnDrawGUI()
	{
		Super::OnDrawGUI();
	}

    CSplitView::CSplitView()
    {

    }

    CSplitView::~CSplitView()
    {

    }

    void CSplitView::Construct()
    {
		Super::Construct();

		left = CreateWidget<CSplitViewContainer>(this, "SplitViewLeft");

		right = CreateWidget<CSplitViewContainer>(this, "SplitViewRight");
    }

	Vec2 CSplitView::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		Vec2 parentSize = Vec2(YGUndefined, YGUndefined);
		if (width > 0 && !IsNan(width))
		{
			if (split > 1 || split < 0)
				split = 0.5f;

			left->sizeConstraint.width = width * split;
			right->sizeConstraint.width = width * (1 - split);
		}
		if (height > 0 && !IsNan(height))
		{
			//left->sizeConstraint.height = right->sizeConstraint.height = height;
		}

		left->SetNeedsStyle();
		left->SetNeedsLayout();
		right->SetNeedsStyle();
		right->SetNeedsLayout();
		left->UpdateLayoutIfNeeded();
		right->UpdateLayoutIfNeeded();

		auto leftSize = left->GetComputedLayoutSize();
		auto rightSize = right->GetComputedLayoutSize();

		return Vec2(leftSize.width + rightSize.width, Math::Max(leftSize.height, rightSize.height));
	}

    void CSplitView::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();
		int count = attachedWidgets.GetSize();
		DrawDefaultBackground();

		GUI::PushChildCoordinateSpace(rect);

		GUI::TableFlags flags = GUI::TableFlags_BordersInnerV | GUI::TableFlags_Resizable;

		if (GUI::BeginTable(rect, GetUuid(), "SplitView", 2, flags))
		{
			GUI::TableSetMinColumnWidth(25);

			// Empty headers
			GUI::TableNextRow();
			GUI::TableNextColumn();
			GUI::TableNextColumn();

			// Actual rows/columns
			GUI::TableNextRow();

			// Column 0
			GUI::TableNextColumn();
			auto pos1 = GUI::GetCursorPos();
			leftSize = GUI::GetContentRegionAvailableSpace().x;

			GUI::PushChildCoordinateSpace(pos1);
			left->Render();
			GUI::PopChildCoordinateSpace();

			// Column 1
			GUI::TableNextColumn();
			auto pos2 = GUI::GetCursorPos();
			rightSize = GUI::GetContentRegionAvailableSpace().x;

			GUI::PushChildCoordinateSpace(pos2);
			right->Render();
			GUI::PopChildCoordinateSpace();

			auto split = leftSize / (leftSize + rightSize);
			if (this->split != split)
			{
				auto owner = this->GetOwner();
				this->split = split;
				SetNeedsLayout();
			}

			GUI::EndTable();
		}

		GUI::PopChildCoordinateSpace();
    }

} // namespace CE::Widgets
