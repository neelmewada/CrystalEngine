#include "CoreWidgets.h"

namespace CE::Widgets
{

    CWindow::CWindow()
    {
        windowTitle = GetName().GetString();
    }

    CWindow::~CWindow()
    {

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

    void CWindow::OnDrawGUI()
    {
        if (isShown)
        {
			GUI::WindowFlags windowFlags = GUI::WF_None;
			if (isFullscreen)
				windowFlags |= GUI::WF_FullScreen | GUI::WF_NoPadding;

            GUI::BeginWindow(windowTitle, &isShown, windowFlags);

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
