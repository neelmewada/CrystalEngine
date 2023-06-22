#include "CoreWidgets.h"

namespace CE::Widgets
{
	CSelectableWidget::CSelectableWidget()
	{

	}

	CSelectableWidget::~CSelectableWidget()
	{

	}

	void CSelectableWidget::OnDrawGUI()
	{
		Vec4 padding{};
		Vec4 borderRadius{};
		Vec2 preferredSize = {};
		Vec2 maxSize = {};
		Vec2 minSize = {};

		for (auto& [property, array] : style.styleMap)
		{
			for (auto& styleValue : array)
			{
				if (property == CStylePropertyType::Padding)
				{
					if (styleValue.state == CStateFlag::Default)
						padding = styleValue.vector;
				}
				else if (property == CStylePropertyType::BorderRadius)
				{
					borderRadius = styleValue.vector;
				}
				else if (property == CStylePropertyType::Width)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							preferredSize.x = styleValue.single;
						else
							preferredSize.x = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().x;
					}
				}
				else if (property == CStylePropertyType::Height)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							preferredSize.y = styleValue.single;
						else
							preferredSize.y = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().y;
					}
				}
				else if (property == CStylePropertyType::MaxWidth)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							maxSize.x = styleValue.single;
						else
							maxSize.x = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().x;
					}
				}
				else if (property == CStylePropertyType::MaxHeight)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							maxSize.y = styleValue.single;
						else
							maxSize.y = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().y;
					}
				}
				else if (property == CStylePropertyType::MinWidth)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							minSize.x = styleValue.single;
						else
							minSize.x = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().x;
					}
				}
				else if (property == CStylePropertyType::MinHeight)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							minSize.y = styleValue.single;
						else
							minSize.y = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().y;
					}
				}
			}
		}

		style.Push(CStylePropertyTypeFlags::Inherited);
		
		auto cursorPos = GUI::GetCursorPos();

		Vec2 childSize{};
		if (attachedWidgets.NonEmpty())
		{
			childSize = GUI::GetItemRectSize();
			if (childSize.x <= 0 || childSize.y <= 0)
			{
				childSize = attachedWidgets[0]->CalculateEstimateSize();
			}
		}

		if (minSize.x > 0 && preferredSize.x > minSize.x)
			preferredSize.x = minSize.x;
		if (minSize.y > 0 && preferredSize.y > minSize.y)
			preferredSize.y = minSize.y;
		if (maxSize.x > 0 && preferredSize.x < maxSize.x)
			preferredSize.x = maxSize.x;
		if (maxSize.y > 0 && preferredSize.y < maxSize.y)
			preferredSize.y = maxSize.y;
		
		style.Pop();

		style.Push();
		if (GUI::SelectableEx(GetUuid(), isSelected, GUI::SelectableFlags_AllowItemOverlap))
		{
			isSelected = true;
		}
		style.Pop();
		PollEvents();

		GUI::SetCursorPos(cursorPos);

		if (attachedWidgets.NonEmpty())
		{
			attachedWidgets[0]->RenderGUI();
		}
	}

	void CSelectableWidget::HandleEvent(CEvent* event)
	{


		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
