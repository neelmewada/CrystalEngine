#include "EditorCore.h"

namespace CE::Editor
{

    EditorDockTabItem::EditorDockTabItem()
    {

    }

    void EditorDockTabItem::Construct()
    {
        Super::Construct();

        Child(
            FAssignNew(FHorizontalStack, content)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
            .Padding(Vec4(1, 0, 1, 0) * 20)
            (
                FAssignNew(FLabel, label)
                .FontSize(14)
                .VAlign(VAlign::Center)
                .HAlign(HAlign::Left)
                .MinWidth(140)
            )
        );
    }

    void EditorDockTabItem::OnPostComputeLayout()
    {
        Super::OnPostComputeLayout();

        Vec2 size = GetComputedSize();
        Vec2 pos = GetComputedPosition();

        String::IsAlphabet('a');
    }

    void EditorDockTabItem::HandleEvent(FEvent* event)
    {
        if (event->IsMouseEvent() && dockTab && dockTab->dockspace)
        {
            FMouseEvent* mouseEvent = static_cast<FMouseEvent*>(event);
            FWidget* dockspace = dockTab->dockspace;

            if (mouseEvent->type == FEventType::MouseEnter)
            {
                isHovered = true;

                dockspace->ApplyStyle();
            }
            else if (mouseEvent->type == FEventType::MouseLeave)
            {
                isHovered = false;

                dockspace->ApplyStyle();
            }
            else if (mouseEvent->type == FEventType::MousePress)
            {
                if (dockspace->IsOfType<EditorDockspace>())
                {
                    EditorDockspace* editorDockspace = static_cast<EditorDockspace*>(dockspace);
                    editorDockspace->SelectTab(this);
                }
                else if (dockspace->IsOfType<EditorMinorDockspace>())
                {
                    EditorMinorDockspace* editorDockspace = static_cast<EditorMinorDockspace*>(dockspace);
                    editorDockspace->SelectTab(this);
                }
            }
        }

        Super::HandleEvent(event);
    }
    
}

