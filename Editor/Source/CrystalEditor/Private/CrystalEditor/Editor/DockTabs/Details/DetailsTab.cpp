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

                    FNew(FVerticalStack)
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

        bool editorExists = editor != nullptr;

        emptyLabel->Enabled(!editorExists);
        //editorContainer->Enabled(editorExists);
    }

    void DetailsTab::SetObjectEditor(ObjectEditor* editor)
    {
        if (this->editor == editor)
            return;

        this->editor = editor;

        bool editorExists = editor != nullptr;

        if (editor)
        {
            //editorContainer->Child(*editor);
        }
        else
        {
            //editorContainer->RemoveChildWidget();
        }

        emptyLabel->Enabled(!editorExists);
        //editorContainer->Enabled(editorExists);
    }

}

