#include "EditorCore.h"

namespace CE::Editor
{

    VectorEditorField::VectorEditorField()
    {

    }

    void VectorEditorField::Construct()
    {
        Super::Construct();

        Child(
            FNew(FHorizontalStack)
            .ContentVAlign(VAlign::Fill)
            .ContentHAlign(HAlign::Fill)
            (
                FAssignNew(NumericEditorField, fieldX)
                .OnTextEdited(FUNCTION_BINDING(this, OnTextEdited))
                .OnTextEditingFinished(FUNCTION_BINDING(this, OnTextEditingFinished))
                .ColorTagVisible(true)
                .ColorTag(Color::Red())
                .VAlign(VAlign::Fill)
                .FillRatio(1.0f),

                FAssignNew(NumericEditorField, fieldY)
                .OnTextEdited(FUNCTION_BINDING(this, OnTextEdited))
                .OnTextEditingFinished(FUNCTION_BINDING(this, OnTextEditingFinished))
                .ColorTagVisible(true)
                .ColorTag(Color::Green())
                .VAlign(VAlign::Fill)
                .FillRatio(1.0f),

                FAssignNew(NumericEditorField, fieldZ)
                .OnTextEdited(FUNCTION_BINDING(this, OnTextEdited))
                .OnTextEditingFinished(FUNCTION_BINDING(this, OnTextEditingFinished))
                .ColorTagVisible(true)
                .ColorTag(Color::Blue())
                .VAlign(VAlign::Fill)
                .FillRatio(1.0f),

                FAssignNew(NumericEditorField, fieldW)
                .OnTextEdited(FUNCTION_BINDING(this, OnTextEdited))
                .OnTextEditingFinished(FUNCTION_BINDING(this, OnTextEditingFinished))
                .ColorTagVisible(true)
                .ColorTag(Color::White())
                .VAlign(VAlign::Fill)
                .FillRatio(1.0f)
            )
        );

        VectorType<Vec3>();
    }

    bool VectorEditorField::CanBind(FieldType* field)
    {
        return field->GetDeclarationType()->IsVectorType();
    }

    void VectorEditorField::UpdateValue()
    {
        if (!IsBound())
            return;

        thread_local HashSet floatVectors = { TYPEID(Vec2), TYPEID(Vec3), TYPEID(Vec4) };
        thread_local HashSet intVectors = { TYPEID(Vec2i), TYPEID(Vec3i), TYPEID(Vec4i) };

        TypeId fieldDeclId = field->GetDeclarationTypeId();
        Object* target = targets[0];

        if (fieldDeclId == TYPEID(Vec2))
        {
            Vec2 value = field->GetFieldValue<Vec2>(target);
            fieldX->Text(String::Format("{}", value.x));
            fieldY->Text(String::Format("{}", value.y));
        }
        else if (fieldDeclId == TYPEID(Vec2i))
        {
            Vec2i value = field->GetFieldValue<Vec2i>(target);
            fieldX->Text(String::Format("{}", value.x));
            fieldY->Text(String::Format("{}", value.y));
        }
        else if (fieldDeclId == TYPEID(Vec3))
        {
            Vec3 value = field->GetFieldValue<Vec3>(target);
            fieldX->Text(String::Format("{}", value.x));
            fieldY->Text(String::Format("{}", value.y));
            fieldZ->Text(String::Format("{}", value.z));
        }
        else if (fieldDeclId == TYPEID(Vec3i))
        {
            Vec3i value = field->GetFieldValue<Vec3i>(target);
            fieldX->Text(String::Format("{}", value.x));
            fieldY->Text(String::Format("{}", value.y));
            fieldZ->Text(String::Format("{}", value.z));
        }
        else if (fieldDeclId == TYPEID(Vec4))
        {
            Vec4 value = field->GetFieldValue<Vec4>(target);
            fieldX->Text(String::Format("{}", value.x));
            fieldY->Text(String::Format("{}", value.y));
            fieldZ->Text(String::Format("{}", value.z));
            fieldW->Text(String::Format("{}", value.w));
        }
        else if (fieldDeclId == TYPEID(Vec4i))
        {
            Vec4i value = field->GetFieldValue<Vec4i>(target);
            fieldX->Text(String::Format("{}", value.x));
            fieldY->Text(String::Format("{}", value.y));
            fieldZ->Text(String::Format("{}", value.z));
            fieldW->Text(String::Format("{}", value.w));
        }
    }

    void VectorEditorField::OnTextEdited(NumericEditorField*)
    {
        if (!IsBound())
            return;

        SetFieldValue();
    }

    void VectorEditorField::OnTextEditingFinished(NumericEditorField*)
    {
        if (!IsBound())
            return;

        SetFieldValue();
    }

    void VectorEditorField::SetFieldValue()
    {
        if (!IsBound())
            return;

        Object* target = targets[0];

        TypeId fieldDeclId = field->GetDeclarationTypeId();

        if (fieldDeclId == TYPEID(Vec2))
        {
            Vec2 value;
            const String& x = fieldX->Text();
            const String& y = fieldY->Text();
            if (String::TryParse(x, value.x) &&
                String::TryParse(y, value.y))
            {
                field->SetFieldValue<Vec2>(target, value);
                target->OnFieldEdited(field->GetName());
            }
        }
        else if (fieldDeclId == TYPEID(Vec2i))
        {
            Vec2i value;
            const String& x = fieldX->Text();
            const String& y = fieldY->Text();
            if (String::TryParse(x, value.x) &&
                String::TryParse(y, value.y))
            {
                field->SetFieldValue<Vec2i>(target, value);
                target->OnFieldEdited(field->GetName());
            }
        }
        else if (fieldDeclId == TYPEID(Vec3))
        {
            Vec3 value;
            const String& x = fieldX->Text();
            const String& y = fieldY->Text();
            const String& z = fieldZ->Text();
            if (String::TryParse(x, value.x) &&
                String::TryParse(y, value.y) &&
                String::TryParse(z, value.z))
            {
                field->SetFieldValue<Vec3>(target, value);
                target->OnFieldEdited(field->GetName());
            }
        }
        else if (fieldDeclId == TYPEID(Vec3i))
        {
            Vec3i value;
            const String& x = fieldX->Text();
            const String& y = fieldY->Text();
            const String& z = fieldZ->Text();
            if (String::TryParse(x, value.x) &&
                String::TryParse(y, value.y) &&
                String::TryParse(z, value.z))
            {
                field->SetFieldValue<Vec3i>(target, value);
                target->OnFieldEdited(field->GetName());
            }
        }
        else if (fieldDeclId == TYPEID(Vec4))
        {
            Vec4 value;
            const String& x = fieldX->Text();
            const String& y = fieldY->Text();
            const String& z = fieldZ->Text();
            const String& w = fieldW->Text();
            if (String::TryParse(x, value.x) &&
                String::TryParse(y, value.y) &&
                String::TryParse(z, value.z) &&
                String::TryParse(w, value.w))
            {
                field->SetFieldValue<Vec4>(target, value);
                target->OnFieldEdited(field->GetName());
            }
        }
        else if (fieldDeclId == TYPEID(Vec4i))
        {
            Vec4i value;
            const String& x = fieldX->Text();
            const String& y = fieldY->Text();
            const String& z = fieldZ->Text();
            const String& w = fieldW->Text();
            if (String::TryParse(x, value.x) &&
                String::TryParse(y, value.y) &&
                String::TryParse(z, value.z) &&
                String::TryParse(w, value.w))
            {
                field->SetFieldValue<Vec4i>(target, value);
                target->OnFieldEdited(field->GetName());
            }
        }
    }

    VectorEditorField::Self& VectorEditorField::VectorType(TypeId type)
    {
        thread_local HashSet floatVectors = { TYPEID(Vec2), TYPEID(Vec3), TYPEID(Vec4) };
        thread_local HashSet intVectors = { TYPEID(Vec2i), TYPEID(Vec3i), TYPEID(Vec4i) };

        if (floatVectors.Exists(type))
        {
            fieldX->NumericType<f32>();
            fieldY->NumericType<f32>();
            fieldZ->NumericType<f32>();
            fieldW->NumericType<f32>();
        }
        else if (intVectors.Exists(type))
        {
            fieldX->NumericType<s32>();
            fieldY->NumericType<s32>();
            fieldZ->NumericType<s32>();
            fieldW->NumericType<s32>();
        }
        else
        {
            return *this;
        }

        fieldX->Enabled(true);
        fieldY->Enabled(true);
        fieldZ->Enabled(false);
        fieldW->Enabled(false);

        if (type == TYPEID(Vec3) || type == TYPEID(Vec3i))
        {
            fieldZ->Enabled(true);
        }
        else if (type == TYPEID(Vec4) || type == TYPEID(Vec4i))
        {
            fieldZ->Enabled(true);
            fieldW->Enabled(true);
        }

        return *this;
    }
}

