#include "CrystalEditor.h"

namespace CE::Editor
{

    DetailsTab::DetailsTab()
    {

    }

    void DetailsTab::Construct()
    {
        Super::Construct();

        (*this)
			.Title("Details")
			.Content(
                FNew(FOverlayStack)
                .VAlign(VAlign::Fill)
                .HAlign(HAlign::Fill)
                (
                    FAssignNew(FLabel, emptyLabel)
                    .Text("Please select an actor in Scene Outliner to see it's properties.")
                    .VAlign(VAlign::Top)
                    .HAlign(HAlign::Center)
                    .Margin(Vec4(0, 50, 0, 0)),

                    FAssignNew(FVerticalStack, editorContainer)
                    .VAlign(VAlign::Fill)
                    .HAlign(HAlign::Fill)
                    (
                        FAssignNew(FLabel, actorName)
                        .Text("Actor Name")
                        .FontSize(14)
                        .HAlign(HAlign::Left),

                        FAssignNew(ComponentTreeView, treeView)
                        .HAlign(HAlign::Fill)
                        .FillRatio(1.0f)

                    )
                )
            )
			.Style("EditorMinorDockTab")
        ;

        
        SetSelectedActor(nullptr);
    }

    void DetailsTab::SetSelectedActor(Actor* actor)
    {
        if (actor)
        {
            treeView->SetActor(actor);
            actorName->Text(actor->GetName().GetString());
        }
        else
        {
            editor = nullptr;
        }

        bool actorExists = actor != nullptr;

        if (editor)
        {
            //editorContainer->Child(*editor);
        }
        else
        {
            //editorContainer->RemoveChildWidget();
        }

        emptyLabel->Enabled(!actorExists);
        editorContainer->Enabled(actorExists);
    }

}

