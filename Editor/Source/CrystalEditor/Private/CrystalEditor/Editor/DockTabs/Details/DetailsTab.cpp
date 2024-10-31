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

                    FAssignNew(FCompoundWidget, detailsContainer)
                    .VAlign(VAlign::Fill)
                    .HAlign(HAlign::Fill)
                    .As<FCompoundWidget>()
                    .Child(
                        FNew(FSplitBox)
                        .Direction(FSplitDirection::Vertical)
                        .SplitterSize(4.0f)
                        .VAlign(VAlign::Fill)
                        .HAlign(HAlign::Fill)
                        .FillRatio(1.0f)
                        (
                            FNew(FVerticalStack)
                            .HAlign(HAlign::Fill)
                            .FillRatio(0.3f)
                            (
                                FAssignNew(FLabel, actorName)
                                .Text("Actor Name")
                                .FontSize(14)
                                .HAlign(HAlign::Left)
                                .Margin(Vec4(5, 10, 5, 10)),

                                FAssignNew(ComponentTreeView, treeView)
                                .OnSelectionChanged(FUNCTION_BINDING(this, OnComponentSelectionChanged))
                                .HAlign(HAlign::Fill)
                                .VAlign(VAlign::Fill)
                                .FillRatio(1.0f)
                            ),

                            FAssignNew(FStyledWidget, editorContainer)
                            .HAlign(HAlign::Fill)
                            .FillRatio(0.7f)
                            .Padding(Vec4(0, 5, 0, 0))
                        )

                    )
                )
            )
			.Style("EditorMinorDockTab")
        ;

        
        SetSelectedActor(nullptr);
    }

    void DetailsTab::OnComponentSelectionChanged(ComponentTreeItem* item)
    {
        editorContainer->RemoveChildWidget();

        if (editor)
        {
            editor->QueueDestroy();
            editor = nullptr;
        }

        if (item && item->GetActor())
        {
            editor = ObjectEditorRegistry::Get().FindOrCreate(item->GetActor());
            editorContainer->Child(*editor);
        }
    }

    void DetailsTab::SetSelectedActor(Actor* actor)
    {
        if (actor)
        {
            treeView->SetActor(actor);
            actorName->Text(actor->GetName().GetString());
        }

        bool actorExists = actor != nullptr;

        emptyLabel->Enabled(!actorExists);
        detailsContainer->Enabled(actorExists);
    }

}

