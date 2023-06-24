#include "CoreWidgets.h"

namespace CE::Widgets
{

	CStackLayout::CStackLayout()
	{

	}

	CStackLayout::~CStackLayout()
	{

	}

	CStackDirection CStackLayout::GetDirection() const
	{
		return direction;
	}

	void CStackLayout::SetDirection(CStackDirection direction)
	{
		this->direction = direction;
	}

	void CStackLayout::OnDrawGUI()
	{
		Vec4 padding{};
		Color backgroundColor{};
		Vec2 preferredSize{};
		Vec2 minSize{};
		Vec2 maxSize{};

		for (auto& [property, array] : style.styleMap)
		{
			for (auto& styleValue : array)
			{
				if (property == CStylePropertyType::Padding && styleValue.state == CStateFlag::Default && 
					styleValue.subControl == CSubControl::None)
				{
					padding = styleValue.vector;
				}
				else if (property == CStylePropertyType::Background && styleValue.state == CStateFlag::Default && 
					styleValue.subControl == CSubControl::None && styleValue.IsColor())
				{
					backgroundColor = styleValue.color;
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

		style.Push();
		GUI::BeginChild("", GetUuid(), preferredSize, 0.0f, minSize, maxSize, padding, backgroundColor, GUI::WF_NoMove | GUI::WF_NoTitleBar);
		style.Pop();

		style.Push(CStylePropertyTypeFlags::Inherited);
		{
			for (int i = 0; i < attachedWidgets.GetSize(); i++)
			{
				attachedWidgets[i]->RenderGUI();
				if (direction == CStackDirection::Horizontal && i != attachedWidgets.GetSize() - 1)
				{
					GUI::SameLine();
				}
			}
		}
		style.Pop();

		style.Push();
		GUI::EndChild();
		style.Pop();

		PollEvents();
	}

	void CStackLayout::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
