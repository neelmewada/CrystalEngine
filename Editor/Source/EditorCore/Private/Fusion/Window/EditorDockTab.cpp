#include "EditorCore.h"

namespace CE::Editor
{

	EditorDockTab::EditorDockTab()
    {

    }

    void EditorDockTab::Construct()
    {
        Super::Construct();

        Child(
            FNew(FVerticalStack)
            .VAlign(VAlign::Fill)
            .HAlign(HAlign::Fill)
            (
                FAssignNew(EditorMenuBar, menuBar)
                .HAlign(HAlign::Fill),

                FAssignNew(EditorToolBar, toolBar)
                .HAlign(HAlign::Fill),

                FAssignNew(FStyledWidget, content)
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill)
                .FillRatio(1.0f)
                .Name("EditorContent")
            )
        );
    }
    
}

