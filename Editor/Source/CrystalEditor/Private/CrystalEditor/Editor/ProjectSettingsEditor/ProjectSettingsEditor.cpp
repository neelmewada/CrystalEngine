#include "CrystalEditor.h"

namespace CE::Editor
{
    static ProjectSettingsEditor* instance = nullptr;
    constexpr f32 headerGap = 20;

    ProjectSettingsEditor::ProjectSettingsEditor()
    {

    }

    void ProjectSettingsEditor::Construct()
    {
        Super::Construct();

        instance = this;

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
                .FillRatio(0.2f)
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
                .FillRatio(0.8f)
            )
        );

    	settingsClasses = Settings::GetAllSettingsClasses();

        left->AddChild(
            FNew(FTextButton)
            .Text("All Settings")
            .FontSize(14)
            .Underline(FPen(Color::White(), 1, FPenStyle::DottedLine))
            .Cursor(SystemCursor::Hand)
            .OnClicked(FUNCTION_BINDING(this, ShowAllSettings))
            .ClipChildren(true)
            .Style("Button.Clear")
            .Margin(Vec4(0, 0, 0, headerGap))
        );

        for (int i = 0; i < settingsClasses.GetSize(); ++i)
        {
            ClassType* clazz = settingsClasses[i];
            int index = i;

            left->AddChild(
                FNew(FTextButton)
                .Text(clazz->GetDisplayName())
                .FontSize(14)
                .Underline(FPen(Color::White(), 1, FPenStyle::DottedLine))
                .Cursor(SystemCursor::Hand)
                .OnClicked([this, index]
                {
                    OnSettingsItemClicked(index);
                })
                .ClipChildren(true)
                .Style("Button.Clear")
            );
        }
    }

    void ProjectSettingsEditor::OnBeginDestroy()
    {
	    Super::OnBeginDestroy();

        if (this == instance)
        {
            instance = nullptr;
        }
    }

    void ProjectSettingsEditor::Show()
    {
        CrystalEditorWindow* editor = CrystalEditorWindow::Get();

        if (instance == nullptr)
        {
            FAssignNewOwned(ProjectSettingsEditor, instance, editor);
            editor->AddDockTab(instance);
        }

        editor->SelectTab(instance);
    }

    void ProjectSettingsEditor::ShowAllSettings()
    {
        if (editor)
        {
            splitRatio = editor->GetSplitRatio();
            editor = nullptr;
        }

        editors.Clear();
        right->DestroyAllChildren();

        for (ClassType* settingsClass : settingsClasses)
        {
            Settings* target = Settings::LoadSettings(settingsClass);
            if (!target)
                continue;

            ObjectEditor* curEditor = ObjectEditorRegistry::Get().Create(target);
            editors.Add(curEditor);

            if (this->editor == nullptr)
            {
                this->editor = curEditor;
            }

            curEditor->FixedInputWidth(180);
            curEditor->SetSplitRatio(splitRatio);

            right->AddChild(curEditor);
        }

        for (ObjectEditor* editor : editors)
        {
            editor->SetEditorGroup(editors);
        }
    }

    void ProjectSettingsEditor::OnSettingsItemClicked(int index)
    {
        if (index < 0 || index >= settingsClasses.GetSize())
            return;

        if (editor)
        {
            splitRatio = editor->GetSplitRatio();
        }

        right->DestroyAllChildren();

        Settings* target = Settings::LoadSettings(settingsClasses[index]);

        editor = ObjectEditorRegistry::Get().Create(target);

        editor->FixedInputWidth(180);
        editor->SetSplitRatio(splitRatio);

        right->AddChild(
            FNew(FLabel)
            .FontSize(18)
            .Text(target->GetTitleName())
            .Margin(Vec4(10, 10, 0, 15))
        );

        right->AddChild(editor);
    }
}

