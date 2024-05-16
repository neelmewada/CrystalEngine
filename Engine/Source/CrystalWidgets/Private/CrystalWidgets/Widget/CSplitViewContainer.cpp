#include "CrystalWidgets.h"

namespace CE::Widgets
{
	constexpr f32 splitterWidth = 3.0f;

	CSplitViewContainer::CSplitViewContainer()
	{
		clipChildren = true;
	}

	CSplitViewContainer::~CSplitViewContainer()
	{
		
	}

	bool CSplitViewContainer::IsLayoutCalculationRoot()
	{
		return true;
	}

	Vec2 CSplitViewContainer::CalculateIntrinsicSize(f32 width, f32 height)
	{
		return Super::CalculateIntrinsicSize(width, height);
	}

	Vec2 CSplitViewContainer::GetComputedLayoutTopLeft()
	{
		if (splitView == nullptr)
			return Super::GetComputedLayoutTopLeft();

		Vec2 pos = Vec2();
		Vec2 parentSize = splitView->GetComputedLayoutSize();
		int index = splitView->containers.IndexOf(this);
		
		if (index < 0)
			return Super::GetComputedLayoutTopLeft();

		for (int i = 0; i < index; i++)
		{
			if (splitView->GetOrientation() == COrientation::Horizontal)
				pos.x += splitView->containers[i]->splitRatio * parentSize.width + splitterWidth;
			else
				pos.y += splitView->containers[i]->splitRatio * parentSize.height + splitterWidth;
		}

		return pos;
	}

	Vec2 CSplitViewContainer::GetComputedLayoutSize()
	{
		if (splitView == nullptr)
			return Super::GetComputedLayoutSize();

		Vec2 parentSize = splitView->GetComputedLayoutSize();
		int index = splitView->containers.IndexOf(this);
		int numChildren = splitView->containers.GetSize();
		if (index < 0)
			return Super::GetComputedLayoutSize();
		
		if (splitView->GetOrientation() == COrientation::Horizontal)
			return Vec2(parentSize.x * splitRatio - (f32)(numChildren - 1) * splitterWidth, parentSize.y);

		return Vec2(parentSize.x, parentSize.y * splitRatio - (f32)(numChildren - 1) * splitterWidth);
	}

} // namespace CE::Widgets
