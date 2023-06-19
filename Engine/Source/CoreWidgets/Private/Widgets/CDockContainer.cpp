#include "CoreWidgets.h"

namespace CE::Widgets
{
	CDockContainer::CDockContainer()
	{

	}

	CDockContainer::~CDockContainer()
	{

	}

	void CDockContainer::OnDrawGUI()
	{
		if (isShown)
		{
			auto windowFlags =  GUI::WF_NoPadding;
			if (isFullscreen)
				windowFlags |= GUI::WF_FullScreen;
			if (hasMenu)
				windowFlags |= GUI::WF_MenuBar;

			style.Push();
			GUI::BeginWindow(windowTitle, nullptr, windowFlags);
			style.Pop();

			if (!disableDock)
				GUI::DockSpace((GUI::ID)GetUuid());

			style.Push(CStylePropertyTypeFlags::Inherited); // Push inheritable properties for sub-widgets

			for (CWidget* subWidget : attachedWidgets)
			{
				subWidget->RenderGUI();
			}

			style.Pop();

			GUI::EndWindow();

			PollEvents();
		}
	}

	void CDockContainer::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
