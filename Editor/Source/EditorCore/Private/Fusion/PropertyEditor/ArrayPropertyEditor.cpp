#include "EditorCore.h"

namespace CE::Editor
{

    ArrayPropertyEditor::ArrayPropertyEditor()
    {

    }

    void ArrayPropertyEditor::ConstructEditor()
    {
        Child(
            FAssignNew(FSplitBox, splitBox)
            .Direction(FSplitDirection::Horizontal)
            .SplitterHoverBackground(Color::Clear())
            .SplitterBackground(Color::RGBA(26, 26, 26))
            .SplitterSize(4.0f)
            .SplitterDrawRatio(0.25f)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
            (
                FAssignNew(FHorizontalStack, left)
                .ContentHAlign(HAlign::Left)
                .ContentVAlign(VAlign::Center)
                .ClipChildren(true)
                .FillRatio(0.35f)
                .Padding(Vec4(2, 1, 2, 1) * 5)
                (
                    FAssignNew(FLabel, fieldNameLabel)
                    .Text("Field Name")
                ),

                FAssignNew(FHorizontalStack, right)
                .ContentHAlign(HAlign::Left)
                .ContentVAlign(VAlign::Center)
                .ClipChildren(true)
                .FillRatio(0.65f)
                .Padding(Vec4(2, 1, 2, 1) * 5)
                (
                    FNew(FTextInput)
                    .Text("Field Editor")
                    .FontSize(13)
                )
            )
        );
    }

}

