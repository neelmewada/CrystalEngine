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

    void PropertyEditor::SetTarget(FieldType* field, const Array<Object*>& targets)
    {
        FieldNameText(field->GetDisplayName());

        
    }

    void PropertyEditor::OnPaint(FPainter* painter)
    {
	    Super::OnPaint(painter);

        painter->SetPen(FPen(Color::RGBA(26, 26, 26), 1));
        painter->SetBrush(FBrush());

        constexpr f32 height = 1.0f;
        Vec2 pos = computedPosition + Vec2(0, computedSize.height - height);
        Vec2 size = Vec2(computedSize.width, height);

        painter->DrawLine(pos, pos + Vec2(size.x, 0));
    }
}

