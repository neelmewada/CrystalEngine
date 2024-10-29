#include "EditorCore.h"

namespace CE::Editor
{

    ComponentTreeViewRow::ComponentTreeViewRow()
    {
        m_Padding = Vec4(1, 1, 1, 1) * 2.5f;
    }

    void ComponentTreeViewRow::Construct()
    {
        Super::Construct();

        FBrush testImage = FBrush("/Engine/Resources/Icons/Warning");

        Child(
            FAssignNew(FHorizontalStack, content)
            .ContentVAlign(VAlign::Center)
            .ContentHAlign(HAlign::Left)
            (
                FAssignNew(FImage, icon)
                .Background(testImage)
                .Width(12)
                .Height(12)
                .VAlign(VAlign::Center),

                FAssignNew(FLabel, label)
                .Text("Label")
                .FontSize(13)
            )
        );
    }
    
}

