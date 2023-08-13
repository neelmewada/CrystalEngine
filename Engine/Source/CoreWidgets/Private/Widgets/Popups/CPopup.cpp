#include "CoreWidgets.h"

namespace CE::Widgets
{

	CPopup::CPopup()
	{
		title = String::Format("{}###{}", GetName(), (u64)GetUuid());
	}

	CPopup::~CPopup()
	{

	}

	void CPopup::Show()
	{
		isShown = true;
		pushShow = true;
	}

	void CPopup::Hide()
	{
		isShown = false;

	}

	Vec2 CPopup::CalculateShowPosition()
	{
		SetNeedsLayout();
		UpdateLayoutIfNeeded();

		auto size = GetComputedLayoutSize();
		auto parentSize = GUI::GetMainViewport()->size;
		if (GetOwner() != nullptr && GetOwner()->IsWindow())
		{
			CWindow* window = (CWindow*)GetOwner();
			auto winSize = window->GetWindowSize();
			if (winSize.x > 0 && winSize.y > 0 && !isnan(winSize.x) && !isnan(winSize.y))
				parentSize = winSize;
		}

		auto pos = parentSize / 2 - size / 2;
		return pos;
	}

	void CPopup::OnDrawGUI()
	{
		if (pushShow)
		{
			pushShow = false;
			GUI::OpenPopup(GetUuid());

			GUI::SetNextWindowPos(CalculateShowPosition());
		}

		if (isShown)
		{
			auto size = GetComputedLayoutSize();
			auto viewportSize = GUI::GetMainViewport()->size;

			if (!isnan(size.x) && !isnan(size.y))
			{
				GUI::SetNextWindowSize(size);
			}

			GUI::WindowFlags flags = GUI::WF_NoMove | GUI::WF_NoResize;
			if (!showsTitle)
				flags |= GUI::WF_NoTitleBar;
			if (isScrollDisabled)
				flags |= GUI::WF_NoScrollWithMouse;

			bool isOpen = GUI::BeginPopup(title, GetUuid(), flags);
			
			if (isOpen)
			{
				if (!isShown)
					GUI::CloseCurrentPopup();

				GUI::PushZeroingChildCoordinateSpace();
				{
					for (auto child : attachedWidgets)
					{
						child->Render();
					}
				}
				GUI::PopChildCoordinateSpace();

				GUI::EndPopup();
				PollEvents();
			}
		}
	}

} // namespace CE::Widgets
