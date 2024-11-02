#include "EditorCore.h"

namespace CE::Editor
{

    PropertyEditor::PropertyEditor()
    {

    }

    void PropertyEditor::Construct()
    {
        Super::Construct();

        Child(
            FAssignNew(FSplitBox, splitBox)
            .Direction(FSplitDirection::Horizontal)
            .SplitterHoverBackground(Color::Clear())
            .SplitterBackground(Color::RGBA(26, 26, 26))
            .SplitterSize(4.0f)
            .SplitterDrawRatio(0.5f)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
            (
                FAssignNew(FHorizontalStack, left)
                .ContentHAlign(HAlign::Left)
                .ContentVAlign(VAlign::Center)
                .ClipChildren(true)
                .FillRatio(0.35f)
                .Name("DebugStack")
                (
                    FAssignNew(FLabel, fieldNameLabel)
                    .Text("Field Name")
                ),

                FAssignNew(FHorizontalStack, right)
                .ContentHAlign(HAlign::Left)
                .ContentVAlign(VAlign::Center)
                .ClipChildren(true)
                .FillRatio(0.65f)
                (
                    FNew(FLabel)
                    .Text("Field Editor")
                )
            )
        );

    }

    void PropertyEditor::SetTarget(FieldType* field, const Array<Object*>& targets)
    {
        FieldNameText(field->GetDisplayName());
    }
}

