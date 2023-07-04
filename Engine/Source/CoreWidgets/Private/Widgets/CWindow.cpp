#include "CoreWidgets.h"

namespace CE::Widgets
{

    CWindow::CWindow()
    {
		SetTitle(GetName().GetCString());
    }

    CWindow::~CWindow()
    {

    }

	Vec2 CWindow::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(windowSize.x, YGUndefined);
	}

	void CWindow::Show()
    {
        isShown = true;
    }

    void CWindow::Hide()
    {
        isShown = false;
    }

    void CWindow::SetTitle(const String& title)
    {
        this->windowTitle = String::Format(title + "###{}", GetName());
    }

	void CWindow::OnComputeStyle()
	{
		
	}

    void CWindow::OnDrawGUI()
    {
        if (isShown)
        {
			GUI::WindowFlags windowFlags = GUI::WF_None;
			if (allowHorizontalScroll)
				windowFlags |= GUI::WF_HorizontalScrollbar;
			if (isFullscreen)
				windowFlags |= GUI::WF_FullScreen | GUI::WF_NoPadding;
			if (IsDockSpaceWindow())
				windowFlags |= GUI::WF_NoPadding;

			auto color = defaultStyleState.background;
			
			if (color.a > 0)
				GUI::PushStyleColor(GUI::StyleCol_WindowBg, color);

            GUI::BeginWindow(windowTitle, &isShown, windowFlags);

			if (color.a > 0)
				GUI::PopStyleColor();

			if (IsDockSpaceWindow())
			{
				if (dockSpaceId.IsEmpty())
					dockSpaceId = String::Format("DockSpace##{}", GetUuid());
				GUI::DockSpace(dockSpaceId);
			}

            for (CWidget* subWidget : attachedWidgets)
            {
                subWidget->RenderGUI();
            }
            
			auto winSize = GUI::GetWindowSize();

            GUI::EndWindow();

			if (windowSize.x > 0 && windowSize.y > 0 && windowSize != winSize)
			{
				CResizeEvent event{};
				event.name = "WindowResized";
				event.type = CEventType::ResizeEvent;
				event.oldSize = windowSize;
				event.newSize = winSize;
				event.sender = this;
				HandleEvent(&event);
			}

			this->windowSize = winSize;

			PollEvents();
        }
    }

	void CWindow::HandleEvent(CEvent* event)
	{
		if (event->GetEventType() == CEventType::ResizeEvent)
		{
			CResizeEvent* resizeEvent = (CResizeEvent*)event;
			emit OnWindowResized(resizeEvent->oldSize, resizeEvent->newSize);

			SetNeedsLayout();
		}

		Super::HandleEvent(event);
	}

    void CWindow::AddSubWidget(CWidget* subWidget)
    {
        AttachSubobject(subWidget);
    }

    void CWindow::RemoveSubWidget(CWidget* subWidget)
    {
        DetachSubobject(subWidget);
    }
    
} // namespace CE::Widgets
