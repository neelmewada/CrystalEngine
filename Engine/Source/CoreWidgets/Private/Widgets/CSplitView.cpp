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
		if (height > 0 && !IsNan(height) && stretchToFill)
		{
			left->sizeConstraint.height = right->sizeConstraint.height = height;
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

	void CSplitView::OnBeforeComputeStyle()
	{
		Super::OnBeforeComputeStyle();

		auto splitter = stylesheet->SelectStyle(this, CStateFlag::Default, CSubControl::Splitter);
		if (splitter.properties.KeyExists(CStylePropertyType::Background))
			splitterColor = splitter.properties[CStylePropertyType::Background].color;
	}

	void CSplitView::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();
		int count = attachedWidgets.GetSize();
		DrawDefaultBackground();

		GUI::TableFlags flags = GUI::TableFlags_BordersInnerV | GUI::TableFlags_Resizable;

		GUI::PushStyleColor(GUI::StyleCol_TableBorderLight, splitterColor);
		GUI::PushStyleColor(GUI::StyleCol_TableBorderStrong, splitterColor);
		GUI::PushStyleColor(GUI::StyleCol_Separator, splitterColor);
		GUI::PushStyleColor(GUI::StyleCol_SeparatorActive, splitterColor);
		GUI::PushStyleColor(GUI::StyleCol_SeparatorHovered, splitterColor);

		if (GUI::BeginTable(rect, GetUuid(), "SplitView", 2, flags))
		{
			float totalWidth = leftSize + rightSize;
			if (firstTime && !IsNan(totalWidth) && totalWidth > 0)
			{
				firstTime = false;
				if (initialSplit > 1 || initialSplit < 0)
					initialSplit = 0.5f;
				GUI::TableSetColumnWidth(0, totalWidth * initialSplit);
			}

			// Actual rows/columns
			GUI::TableNextRow();

			// Column 0
			GUI::TableNextColumn();
			
			auto pos1 = GUI::GetCursorPos();
			pos1.y = 0;
			leftSize = GUI::GetContentRegionAvailableSpace().x;

			GUI::PushChildCoordinateSpace(pos1);
			left->Render();
			GUI::PopChildCoordinateSpace();

			// Column 1
			GUI::TableNextColumn();
			auto pos2 = GUI::GetCursorPos();
			pos2.y = 0;
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

		GUI::PopStyleColor(5);
    }

} // namespace CE::Widgets
