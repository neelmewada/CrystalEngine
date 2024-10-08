#include "EditorCore.h"

namespace CE::Editor
{

    EditorMinorDockspace::EditorMinorDockspace()
    {

    }

    void EditorMinorDockspace::Construct()
    {
        Super::Construct();

        Child(
            FAssignNew(FVerticalStack, rootBox)
            .ContentHAlign(HAlign::Fill)
            .ContentVAlign(VAlign::Top)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
            .Name("RootBox")
            (
                FAssignNew(FStyledWidget, titleBar)
                .Background(Color::Black())
                .HAlign(HAlign::Fill)
                .Height(30)
                (
                    FAssignNew(FHorizontalStack, tabWell)
                    .ContentHAlign(HAlign::Left)
                    .VAlign(VAlign::Fill)
                    .Name("TabWell")
                ),

                FAssignNew(FStyledWidget, content)
                .Padding(Vec4(0, 0, 0, 0))
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill)
                .FillRatio(1.0f)
                .Name("Content")
            )
        );
    }
    
}

