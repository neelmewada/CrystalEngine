#include "CoreWidgets.h"

namespace CE::Widgets
{

	CGridLayout::CGridLayout()
	{

	}

	CGridLayout::~CGridLayout()
	{

	}

	void CGridLayout::OnDrawGUI()
	{
		GUI::BeginGroup();
		Vec2 prevSize{};

		style.Push(CStylePropertyTypeFlags::Inherited);
		{
			for (int i = 0; i < attachedWidgets.GetSize(); i++)
			{
				auto cursor = GUI::GetCursorPos();
				f32 availSpace = GUI::GetContentRegionAvailableSpace().x;

				attachedWidgets[i]->RenderGUI();
				Vec2 size = GUI::GetItemRectSize();
				
				if (i != attachedWidgets.GetSize() - 1 && availSpace > size.x + prevSize.x)
				{
					GUI::SameLine();
					prevSize = size;
				}
				else
				{
					prevSize = {};
				}
			}
		}
		style.Pop();

		style.Push();
		GUI::EndGroup();
		style.Pop();

		PollEvents();
	}

	void CGridLayout::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
