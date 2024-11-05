#include "CrystalEditor.h"

namespace CE::Editor
{

    ProjectSettingsEditor::ProjectSettingsEditor()
    {

    }

    void ProjectSettingsEditor::Construct()
    {
        Super::Construct();

        Title("Project Settings");

        ToolBarEnabled(true);
        MenuBarEnabled(false);

        Content(
            FNew(FSplitBox)
            .Direction(FSplitDirection::Horizontal)
            .SplitterBackground(Color::RGBA(10, 10, 10))
            .SplitterDrawRatio(0.5f)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
            (
                FNew(FScrollBox)
                .HorizontalScroll(false)
                .VerticalScroll(true)
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill)
                .FillRatio(0.3f)
                (
                    FAssignNew(FVerticalStack, left)
                    .ContentHAlign(HAlign::Left)
                    .VAlign(VAlign::Top)
                    .HAlign(HAlign::Fill)
                    .Padding(Vec4(0, 10, 0, 10))
                ),

                FNew(FStyledWidget)
                .Background(Color::RGBA(36, 36, 36))
                .Child(
                    FNew(FScrollBox)
                    .HorizontalScroll(false)
                    .VerticalScroll(true)
                    .VAlign(VAlign::Fill)
                    .HAlign(HAlign::Fill)
                    (
                        FAssignNew(FVerticalStack, right)
                        .ContentHAlign(HAlign::Fill)
                        .VAlign(VAlign::Top)
                        .HAlign(HAlign::Fill)
                    )
                )
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill)
                .FillRatio(0.7f)
            )
        );

        const Array<ClassType*>& settingsClasses = Settings::GetAllSettingsClasses();

        for (int i = 0; i < settingsClasses.GetSize(); ++i)
        {
            ClassType* clazz = settingsClasses[i];

            left->AddChild(
                FNew(FTextButton)
                .Text(clazz->GetDisplayName())
                .FontSize(13)
                .Underline(FPen(Color::White(), 1, FPenStyle::DottedLine))
                .Cursor(SystemCursor::Hand)
                .ClipChildren(true)
                .Style("Button.Clear")
            );
        }
    }
    
}

