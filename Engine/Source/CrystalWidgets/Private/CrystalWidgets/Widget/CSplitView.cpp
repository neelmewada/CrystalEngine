#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CSplitView::CSplitView()
	{
		
	}

	CSplitView::~CSplitView()
	{
		
	}

	bool CSplitView::IsSubWidgetAllowed(Class* subWidgetClass)
	{
		return subWidgetClass != nullptr && subWidgetClass->IsSubclassOf<CSplitViewContainer>();
	}

	void CSplitView::SetOrientation(COrientation orientation)
	{
		this->orientation = orientation;

		SetNeedsStyle();
		SetNeedsLayout();
		SetNeedsPaint();
	}

	bool CSplitView::IsLayoutCalculationRoot()
	{
		return true;
	}

	void CSplitView::AddSplit(f32 ratio)
	{
		if (containers.IsEmpty())
		{
			CSplitViewContainer* containerA = CreateObject<CSplitViewContainer>(this, "SplitContainer");
			CSplitViewContainer* containerB = CreateObject<CSplitViewContainer>(this, "SplitContainer");

			containerA->splitRatio = 1.0f - ratio;
			containerB->splitRatio = ratio;

			UpdateSplits();
			return;
		}

		CSplitViewContainer* last = containers.Top();
		f32 totalRatio = last->splitRatio;
		last->splitRatio = (1.0f - ratio) * totalRatio;

		CSplitViewContainer* next = CreateObject<CSplitViewContainer>(this, "SplitContainer");
		next->splitRatio = ratio * totalRatio;
	}

	void CSplitView::OnPaint(CPaintEvent* paintEvent)
	{
		Super::OnPaint(paintEvent);

	}

	void CSplitView::HandleEvent(CEvent* event)
	{
		if (event->IsMouseEvent())
		{
			
		}

		Super::HandleEvent(event);
	}

	void CSplitView::OnSubobjectAttached(Object* subobject)
	{
		Super::OnSubobjectAttached(subobject);

		if (subobject->IsOfType<CSplitViewContainer>())
		{
			CSplitViewContainer* container = static_cast<CSplitViewContainer*>(subobject);
			container->splitView = this;

			if (containers.GetSize() >= 10) // Only 10 max split views allowed
			{
				RemoveSubWidget(container);
				return;
			}

			containers.Add(container);

			UpdateSplits();
		}
	}

	void CSplitView::OnSubobjectDetached(Object* subobject)
	{
		Super::OnSubobjectDetached(subobject);

		if (subobject->IsOfType<CSplitViewContainer>())
		{
			CSplitViewContainer* container = static_cast<CSplitViewContainer*>(subobject);
			container->splitView = nullptr;

			containers.Remove(container);

			UpdateSplits();
		}
	}

	void CSplitView::UpdateSplits()
	{
		f32 remainingRatio = 1.0f;
		f32 maxRatio = 0;
		int maxRatioIndex = -1;

		for (int i = 0; i < containers.GetSize(); i++)
		{
			auto container = containers[i];
			if (container->splitRatio > maxRatio)
			{
				maxRatio = container->splitRatio;
				maxRatioIndex = i;
			}

			remainingRatio -= container->splitRatio;
		}

		if (remainingRatio < 0)
		{
			
		}
	}

} // namespace CE::Widgets
