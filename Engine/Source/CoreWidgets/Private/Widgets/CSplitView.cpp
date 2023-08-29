#include "CoreWidgets.h"

static const CE::String css = R"(

)";

namespace CE::Widgets
{
    CSplitView::CSplitView()
    {

    }

    CSplitView::~CSplitView()
    {

    }

    void CSplitView::Construct()
    {
		Super::Construct();

		SetStyleSheet(css);

		left = CreateWidget<CLayoutGroup>(this, "SplitViewLeft");

		right = CreateWidget<CLayoutGroup>(this, "SplitViewRight");
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

    void CSplitView::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();
		int count = attachedWidgets.GetSize();

		GUI::PushChildCoordinateSpace(rect);

		GUI::TableFlags flags = GUI::TableFlags_BordersInnerV;

		if (GUI::BeginTable(rect, GetUuid(), "SplitView", 2, flags))
		{
			// Empty headers
			GUI::TableNextRow();
			GUI::TableNextColumn();
			GUI::TableNextColumn();

			// Actual rows/columns


			GUI::EndTable();
		}

		GUI::PopChildCoordinateSpace();
    }

} // namespace CE::Widgets
