#include "EditorCore.h"

namespace CE::Editor
{

    PropertyEditor::PropertyEditor()
    {
        m_HoverRowBackground = Color::RGBA(47, 47, 47);
    }

    void PropertyEditor::Construct()
    {
        Super::Construct();

        ConstructEditor();

        if (splitBox)
        {
            (*splitBox)
                .SplitterHoverBackground(Color::Clear())
                .SplitterBackground(Color::RGBA(26, 26, 26))
                .SplitterSize(4.0f)
                .SplitterDrawRatio(0.25f)
        	;
        }
    }

    void PropertyEditor::ConstructDefaultEditor()
    {
        FBrush caretDown = FBrush("/Engine/Resources/Icons/CaretDown");


        Child(
            FAssignNew(FVerticalStack, contentStack)
            .Padding(Vec4())
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
            (
                FNew(FButton)
                .OnDoubleClicked(FUNCTION_BINDING(this, ToggleExpansion))
                .HAlign(HAlign::Fill)
                .Padding(Vec4())
                .Style("Button.DetailsRow")
                .Name("DetailsRow")
                (
                    FAssignNew(FSplitBox, splitBox)
                    .OnSplitterDragged([this](FSplitBox*)
                    {
                        if (objectEditor)
                        {
                            objectEditor->SetSplitRatio(left->FillRatio(), splitBox);
                        }
                    })
                    .Direction(FSplitDirection::Horizontal)
                    .SplitterHoverBackground(Color::Clear())
                    .SplitterBackground(Color::RGBA(26, 26, 26))
                    .SplitterSize(4.0f)
                    .SplitterDrawRatio(0.25f)
                    .HAlign(HAlign::Fill)
                    (
                        FAssignNew(FHorizontalStack, left)
                        .ContentHAlign(HAlign::Left)
                        .ContentVAlign(VAlign::Center)
                        .ClipChildren(true)
                        .FillRatio(0.35f)
                        .Padding(Vec4(2, 1.0f, 2, 1.0f) * 5)
                        .MinHeight(20)
                        (
                            FAssignNew(FImageButton, expansionArrow)
                            .Image(caretDown)
                            .ImageWidth(11)
                            .ImageHeight(11)
                            .OnClicked(FUNCTION_BINDING(this, ToggleExpansion))
                            .VAlign(VAlign::Center)
                            .Margin(Vec4(0, 0, 5, 0))
                            .Style("ExpandCaretButton")
                            .Visible(IsExpandable()),

                            FAssignNew(FLabel, fieldNameLabel)
                            .Text("Field Name")
                        ),

                        FAssignNew(FHorizontalStack, right)
                        .ContentHAlign(HAlign::Left)
                        .ContentVAlign(VAlign::Center)
                        .ClipChildren(true)
                        .FillRatio(0.65f)
                        .Padding(Vec4(2, 1, 2, 1) * 5)
                        .MinHeight(20)
                    )
                ),

                FAssignNew(FVerticalStack, expansionStack)
                .HAlign(HAlign::Fill)
                .Padding(Vec4())
                .Enabled(IsExpandable() && isExpanded)
            )
        );

        UpdateExpansion();
    }

    void PropertyEditor::ConstructEditor()
    {
        ConstructDefaultEditor();
    }

    bool PropertyEditor::IsFieldSupported(FieldType* field) const
    {
        return IsFieldSupported(field->GetDeclarationTypeId());
    }

    bool PropertyEditor::IsFieldSupported(TypeId fieldTypeId) const
    {
        if (fieldTypeId == TYPEID(Array<>))
            return false;

        thread_local HashSet supportedFields = {
            TYPEID(bool),
            TYPEID(s8), TYPEID(u8),
            TYPEID(s16), TYPEID(u16),
            TYPEID(s32), TYPEID(u32),
            TYPEID(s64), TYPEID(u64),
            TYPEID(f32), TYPEID(f64),
            TYPEID(String), TYPEID(CE::Name),
            TYPEID(Vec2), TYPEID(Vec2i),
            TYPEID(Vec3), TYPEID(Vec3i),
            TYPEID(Vec4), TYPEID(Vec4i),
        };

        if (supportedFields.Exists(fieldTypeId))
            return true;

        TypeInfo* type = GetTypeInfo(fieldTypeId);
        if (type == nullptr)
            return false;

        return type->IsEnum();
    }

    bool PropertyEditor::IsExpandable()
    {
        return false;
    }

    void PropertyEditor::UpdateValue()
    {
        if (editorField)
        {
            editorField->UpdateValue();
        }
    }

    void PropertyEditor::InitTarget(FieldType* field, const Array<Object*>& targets, const Array<void*>& instances)
    {
        FieldNameText(field->GetDisplayName());

        right->DestroyAllChildren();

        auto printError = [&](const String& msg)
            {
                right->AddChild(
                    FNew(FLabel)
                    .FontSize(12)
                    .Text("Error: " + msg)
                    .Foreground(Color::Red())
                );
            };

        if (targets.GetSize() > 1)
        {
            printError("Multiple objects selected!");
	        return;
        }
        if (targets.GetSize() == 0)
        {
            printError("No objects selected!");
            return;
        }

        TypeInfo* fieldDeclType = field->GetDeclarationType();
        if (fieldDeclType == nullptr)
        {
            printError("Cannot find field type!");
	        return;
        }

        this->field = field;
        this->target = targets[0];
        this->instance = instances[0];

        isExpanded = PropertyEditorRegistry::Get()->IsExpanded(field);
        UpdateExpansion();

        TypeId fieldDeclId = fieldDeclType->GetTypeId();
        
        if (fieldDeclType->IsVectorType())
        {
            right->AddChild(
				FNew(VectorEditorField)
                .VectorType(fieldDeclType->GetTypeId())
                .Assign(editorField)
                .BindField(field, targets[0], instances[0])
                .FillRatio(1.0f)
            );
        }
        else if (field->IsNumericField())
        {
            right->AddChild(
                FNew(NumericEditorField)
                .NumericType(fieldDeclType->GetTypeId())
                .ColorTagVisible(false)
                .Assign(editorField)
                .BindField(field, targets[0], instances[0])
                .FillRatio(1.0f)
            );
        }
        else if (fieldDeclType->IsEnum())
        {
            right->AddChild(
				FNew(EnumEditorField)
                .Assign(editorField)
                .BindField(field, targets[0], instances[0])
                .FillRatio(1.0f)
            );
        }
        else if (field->GetDeclarationTypeId() == TYPEID(bool))
        {
            right->AddChild(
				FNew(BoolEditorField)
                .Assign(editorField)
                .BindField(field, targets[0], instances[0])
                .HAlign(HAlign::Left)
                .VAlign(VAlign::Center)
            );
        }
        else if (fieldDeclId == TYPEID(String) || fieldDeclId == TYPEID(CE::Name))
        {
            right->AddChild(
                FNew(TextEditorField)
                .Assign(editorField)
                .BindField(field, targets[0], instances[0])
                .HAlign(HAlign::Left)
                .VAlign(VAlign::Center)
            );
        }
    }

    void PropertyEditor::UpdateTarget(FieldType* field, const Array<Object*>& targets, const Array<void*>& instances)
    {
        this->field = field;
        this->target = targets[0];
        this->instance = instances[0];

        if (editorField)
        {
            editorField->field = field;
            editorField->targets = targets;
            editorField->instances = instances;
        }
    }

    void PropertyEditor::OnPaint(FPainter* painter)
    {
	    Super::OnPaint(painter);

        painter->SetPen(FPen(Color::RGBA(26, 26, 26), 1));
        painter->SetBrush(FBrush());

        constexpr f32 height = 1.0f;
        Vec2 pos = computedPosition + Vec2(0, computedSize.height - height - expansionStack->GetComputedSize().height);
        Vec2 size = Vec2(computedSize.width, height);

        painter->DrawLine(pos, pos + Vec2(size.x, 0));
    }

    void PropertyEditor::SetIndentationLevel(int value)
    {
        indentation = value;

        left->Translation(Vec2(indentation * 10, 0));
    }

    f32 PropertyEditor::GetSplitRatio() const
    {
        return left->FillRatio();
    }

    void PropertyEditor::SetSplitRatio(f32 ratio)
    {
        ratio = Math::Clamp(ratio, 0.01f, 0.99f);
        left->FillRatio(ratio);
        right->FillRatio(1.0f - ratio);
    }

    void PropertyEditor::SetSplitRatio(f32 ratio, FSplitBox* excluding)
    {
        if (splitBox != excluding)
        {
            SetSplitRatio(ratio);
        }
    }

    PropertyEditor::Self& PropertyEditor::FixedInputWidth(f32 width)
    {
        if (editorField != nullptr)
        {
	        editorField->FixedInputWidth(width);
        }

        return *this;
    }

    void PropertyEditor::ToggleExpansion()
    {
        if (!IsExpandable())
            return;

        isExpanded = !isExpanded;

        UpdateExpansion();

        if (isExpanded)
        {
            PropertyEditorRegistry::Get()->ExpandField(field);
            OnExpand();
        }
        else
        {
            PropertyEditorRegistry::Get()->CollapseField(field);
            OnCollapse();
        }
    }

    void PropertyEditor::UpdateExpansion()
    {
        if (!IsExpandable())
            return;

        expansionArrow->GetImage()->Angle(isExpanded ? 0 : -90);
        expansionStack->Enabled(isExpanded);
    }

    
}

