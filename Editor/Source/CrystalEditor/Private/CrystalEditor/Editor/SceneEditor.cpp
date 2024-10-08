#include "CrystalEditor.h"

namespace CE::Editor
{

    SceneEditor::SceneEditor()
    {

    }

    void SceneEditor::Construct()
    {
        Super::Construct();

        Title("Scene Editor");

        (*menuBar)
			.Content(
                FNew(FMenuItem)
                .Text("File")
                .SubMenu(
                    FNew(FMenuPopup)
                    .Name("FileMenu")
                    .As<FMenuPopup>()
                    .Gap(0)
                    .Content(
                        FNew(FMenuItem)
                        .Text("New"),

                        FNew(FMenuItem)
                        .Text("Open"),

                        FNew(FMenuItem)
                        .Text("Open As..."),

                        FNew(FMenuItem)
                        .Text("Open Recent...")
                        .SubMenu(
                            FNew(FMenuPopup)
                            .Gap(0)
                            .Content(
                                FNew(FMenuItem)
                                .Text("Project 1"),

                                FNew(FMenuItem)
                                .Text("Project 2"),

                                FNew(FMenuItem)
                                .Text("Project 3"),

                                FNew(FMenuItem)
                                .Text("Project 4")
                            )
                            .Name("RecentsMenu")
                            .As<FMenuPopup>()
                        ),

                        FNew(FMenuItem)
                        .Text("Save"),

                        FNew(FMenuItem)
                        .Text("Save As..."),

                        FNew(FMenuItem)
                        .Text("Exit")
                        .OnClick([this]
                        {
                            GetContext()->QueueDestroy();
                        })
                    )
                )
                .Name("FileMenuItem"),

                FNew(FMenuItem)
                .Text("Edit")
                .Name("EditMenuItem"),

                FNew(FMenuItem)
                .Text("Help")
                .Name("HelpMenuItem")
            )
            ;
    }
    
}

