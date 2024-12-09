#include "EditorCore.h"

namespace CE::Editor
{

    EditorDockTabItem::EditorDockTabItem()
    {

    }

    void EditorDockTabItem::Construct()
    {
        Super::Construct();

        static FBrush cross = FBrush("/Engine/Resources/Icons/CrossIcon");
        constexpr f32 crossSize = 10;

        Child(
            FAssignNew(FHorizontalStack, content)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
            .Padding(Vec4(1, 0, 1, 0) * 20)
            (
                FAssignNew(FLabel, label)
                .FontSize(12)
                .VAlign(VAlign::Center)
                .HAlign(HAlign::Left)
                .MinWidth(140),

                FNew(FWidget)
                .FillRatio(1.0f),

                FAssignNew(FImageButton, closeButton)
                .Image(cross)
                .ImageWidth(crossSize)
                .ImageHeight(crossSize)
                .OnClicked([this]
                {
                    dockTab->QueueDestroy();
                })
                .VAlign(VAlign::Center)
                .Style("Button.CloseTab")
            )
        );
    }

    void EditorDockTabItem::OnPostComputeLayout()
    {
        Super::OnPostComputeLayout();

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
            else if (mouseEvent->type == FEventType::MousePress && event->sender == this)
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

