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

		for (auto& [property, array] : style.styleMap)
		{
			for (auto& styleValue : array)
			{
				if (property == CStylePropertyType::Padding && styleValue.state == CStateFlag::Default)
				{
					padding = styleValue.vector;
				}
			}
		}

		GUI::SetCursorPos(GUI::GetCursorPos() + Vec2(padding.left, padding.top));

		GUI::BeginGroup();

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
		GUI::EndGroup(padding);
		style.Pop();

		PollEvents();
	}

	void CStackLayout::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
