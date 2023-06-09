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
		GUI::BeginGroup();

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

		GUI::EndGroup();

		PollEvents();
	}

	void CStackLayout::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
