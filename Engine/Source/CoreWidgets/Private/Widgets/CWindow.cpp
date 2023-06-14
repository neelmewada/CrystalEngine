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
			style.Push();
            GUI::BeginWindow(windowTitle, &isShown);
			style.Pop();

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

	void CWindow::HandleEvent(CEvent* event)
	{
		if (event->GetEventType() == CEventType::FocusChanged)
		{

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
