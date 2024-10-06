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
            FNew(FHorizontalStack)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
            .Padding(Vec4(1, 0, 1, 0) * 20)
            (
                FAssignNew(FLabel, label)
                .FontSize(14)
                .VAlign(VAlign::Center)
                .HAlign(HAlign::Left)
                .MinWidth(160)
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
        if (event->IsMouseEvent())
        {
            FMouseEvent* mouseEvent = static_cast<FMouseEvent*>(event);

            if (mouseEvent->type == FEventType::MouseEnter)
            {
                isHovered = true;

                if (GetParent())
                {
                    GetParent()->ApplyStyle();
                }
            }
            else if (mouseEvent->type == FEventType::MouseLeave)
            {
                isHovered = false;

                if (GetParent())
                {
                    GetParent()->ApplyStyle();
                }
            }
            else if (mouseEvent->type == FEventType::MousePress)
            {
                isActive = true;

                if (GetParent() && GetParent()->IsOfType<EditorDockspace>())
                {
                    EditorDockspace* editorDockspace = static_cast<EditorDockspace*>(GetParent());
                    editorDockspace->SelectTab(this);
                }
            }
        }

        Super::HandleEvent(event);
    }
    
}

