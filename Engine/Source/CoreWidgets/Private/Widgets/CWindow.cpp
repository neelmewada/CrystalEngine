#include "CoreWidgets.h"

namespace CE::Widgets
{

    CWindow::CWindow()
    {
        windowTitle = GetName().GetString();
    }

    CWindow::~CWindow()
    {
        CE_LOG(Info, All, "Window Widget Destroyed");
    }

    void CWindow::Show()
    {
        isShown = true;
    }

    void CWindow::Hide()
    {
        isShown = false;
    }

    void CWindow::OnDrawGUI()
    {
        if (isShown)
        {
            GUI::BeginWindow(windowTitle, &isShown);
            
            for (CWidget* subWidget : attachedWidgets)
            {
                subWidget->RenderGUI();
            }
            
            GUI::EndWindow();
        }
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
