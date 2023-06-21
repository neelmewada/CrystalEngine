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
		style.Push();

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



		style.Pop();
	}

	void CSelectableWidget::HandleEvent(CEvent* event)
	{


		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
