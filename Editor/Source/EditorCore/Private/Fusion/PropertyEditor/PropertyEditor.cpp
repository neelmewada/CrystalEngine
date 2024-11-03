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

    bool PropertyEditor::IsFieldSupported(FieldType* field)
    {
        thread_local HashSet<TypeId> supportedFields = {
            
        };

        if (field->GetDeclarationType() == nullptr)
            return false;

        return field->IsNumericField() || field->IsStringField() ||
            field->GetDeclarationType()->IsVectorType() || field->IsEnumField() ||
            supportedFields.Exists(field->GetDeclarationTypeId());
    }

    void PropertyEditor::SetTarget(FieldType* field, const Array<Object*>& targets)
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

        thread_local HashSet<TypeId> floatVectors = { TYPEID(Vec2), TYPEID(Vec3), TYPEID(Vec4) };
        thread_local HashSet<TypeId> intVectors = { TYPEID(Vec2i), TYPEID(Vec3i), TYPEID(Vec4i) };

        if (fieldDeclType->IsVectorType())
        {
            right->AddChild(
				FNew(VectorInputField)
                .VectorType(fieldDeclType->GetTypeId())
                .FillRatio(1.0f)
            );
        }
        else if (field->IsNumericField())
        {
            right->AddChild(
                FNew(NumericInputField)
                .NumericType(fieldDeclType->GetTypeId())
                .ColorTagVisible(false)
                .FillRatio(1.0f)
            );
        }
        else if (fieldDeclType->IsEnum())
        {
	        auto enumType = static_cast<EnumType*>(fieldDeclType);

            // TODO: Add combo box
        }
        else if (field->GetDeclarationTypeId() == TYPEID(bool))
        {
	        // TODO: Add check box
        }
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

