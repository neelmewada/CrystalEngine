#include "CrystalWidgets.h"

namespace CE::Editor
{
    static const HashSet<TypeId> floatVectorTypes = { TYPEID(Vec2), TYPEID(Vec3), TYPEID(Vec4) };

	void VectorComponentInput::OnFocusLost()
	{
		Super::OnFocusLost();

        if (GetText().IsEmpty() || GetText() == "-" || GetText() == "+")
        {
            SetText("0");
        }
	}

	void VectorComponentInput::Construct()
	{
		Super::Construct();

        SetPrecision(4);

        SetInputValidator(CFloatInputValidator);
        SetText("0");
        SetSelectAllOnEdit(true);
	}

	void VectorComponentInput::OnPaintOverlay(CPaintEvent* paintEvent)
	{
		Super::OnPaintOverlay(paintEvent);

        CPainter* painter = paintEvent->painter;

        Vec2 pos = GetComputedLayoutTopLeft();
        Vec2 size = GetComputedLayoutSize();

        f32 padding = 3.0f;
        f32 width = 5.0f;

        Rect rect = Rect::FromSize(pos + Vec2(padding, padding), Vec2(width, size.height - padding * 2));

        if (tagColor.a > 0)
        {
            painter->SetBrush(CBrush(tagColor));
            painter->SetPen(CPen());

            painter->DrawRoundedRect(rect, Vec4(2.5f, 0, 0, 2.5f));
        }
	}

    void VectorFieldEditor::SetVectorType(TypeId vectorTypeId)
    {
	    if (vectorTypeId == TYPEID(Vec2))
	    {
            SetVectorType<Vec2>();
	    }
        else if (vectorTypeId == TYPEID(Vec2i))
        {
            SetVectorType<Vec2i>();
        }
        else if (vectorTypeId == TYPEID(Vec3))
        {
            SetVectorType<Vec3>();
        }
        else if (vectorTypeId == TYPEID(Vec3i))
        {
            SetVectorType<Vec3i>();
        }
        else if (vectorTypeId == TYPEID(Vec4))
        {
            SetVectorType<Vec4>();
        }
        else if (vectorTypeId == TYPEID(Vec4i))
        {
            SetVectorType<Vec4i>();
        }
    }

    Vec4 VectorFieldEditor::GetVectorValue()
    {
        Vec4 result{};
        String::TryParse(fieldX->GetText(), result.x);
        String::TryParse(fieldY->GetText(), result.y);
        String::TryParse(fieldZ->GetText(), result.z);
        String::TryParse(fieldW->GetText(), result.w);
        return result;
    }

    Vec4i VectorFieldEditor::GetVectorIntValue()
    {
        Vec4i result{};
        String::TryParse(fieldX->GetText(), result.x);
        String::TryParse(fieldY->GetText(), result.y);
        String::TryParse(fieldZ->GetText(), result.z);
        String::TryParse(fieldW->GetText(), result.w);
        return result;
    }

    void VectorFieldEditor::SetVectorValue(const Vec4& value)
    {
        fieldX->SetText(String::Format("{}", value.x));
        fieldY->SetText(String::Format("{}", value.y));
        fieldZ->SetText(String::Format("{}", value.z));
        fieldW->SetText(String::Format("{}", value.w));
    }

    void VectorFieldEditor::SetVectorIntValue(const Vec4i& value)
    {
        fieldX->SetText(String::Format("{}", value.x));
        fieldY->SetText(String::Format("{}", value.y));
        fieldZ->SetText(String::Format("{}", value.z));
        fieldW->SetText(String::Format("{}", value.w));
    }

    void VectorFieldEditor::BindField(FieldType* field, void* instance)
    {
        if (!field || !instance)
            return;

        TypeInfo* declType = field->GetDeclarationType();
        if (declType == nullptr)
            return;

        if (!declType->IsVectorType())
            return;

        TypeId declTypeId = declType->GetTypeId();

        this->field = field;
        this->instance = instance;

        Vec4 vec4Value{};
        Vec4i vec4IntValue{};

        if (declTypeId == TYPEID(Vec2))
        {
            vec4Value = field->GetFieldValue<Vec2>(instance);
        }
        else if (declTypeId == TYPEID(Vec3))
        {
            vec4Value = field->GetFieldValue<Vec3>(instance);
        }
        else if (declTypeId == TYPEID(Vec4))
        {
            vec4Value = field->GetFieldValue<Vec4>(instance);
        }
        else if (declTypeId == TYPEID(Vec2i))
        {
            vec4IntValue = field->GetFieldValue<Vec2i>(instance);
        }
        else if (declTypeId == TYPEID(Vec3i))
        {
            vec4IntValue = field->GetFieldValue<Vec3i>(instance);
        }
        else if (declTypeId == TYPEID(Vec4i))
        {
            vec4IntValue = field->GetFieldValue<Vec4i>(instance);
        }

        if (floatVectorTypes.Exists(declTypeId))
        {
            SetVectorValue(vec4Value);
        }
        else
        {
            SetVectorIntValue(vec4IntValue);
        }

        Delegate<void(CTextInput*)> callback = [this, field, instance](CTextInput*)
            {
                if (!fieldX->IsEditing() &&
                    !fieldY->IsEditing() &&
                    !fieldZ->IsEditing() &&
                    !fieldW->IsEditing())
                {
                    if (field && instance)
                    {
                        Vec4 vec4Value = GetVectorValue();

                        TypeId declId = field->GetDeclarationTypeId();

                        if (declId == TYPEID(Vec2))
                        {
                            field->SetFieldValue<Vec2>(instance, vec4Value);
                        }
                        else if (declId == TYPEID(Vec3))
                        {
                            field->SetFieldValue<Vec3>(instance, vec4Value);
                        }
                        else if (declId == TYPEID(Vec4))
                        {
                            field->SetFieldValue<Vec4>(instance, vec4Value);
                        }
                        else if (declId == TYPEID(Vec2i))
                        {
                            field->SetFieldValue<Vec2i>(instance, vec4Value.ToVec4i());
                        }
                        else if (declId == TYPEID(Vec3i))
                        {
                            field->SetFieldValue<Vec3i>(instance, vec4Value.ToVec4i());
                        }
                        else if (declId == TYPEID(Vec4i))
                        {
                            field->SetFieldValue<Vec4i>(instance, vec4Value.ToVec4i());
                        }
                    }
                }
            };

        Bind(fieldX, MEMBER_FUNCTION(VectorComponentInput, OnEditingFinished), callback);
        Bind(fieldY, MEMBER_FUNCTION(VectorComponentInput, OnEditingFinished), callback);
        Bind(fieldZ, MEMBER_FUNCTION(VectorComponentInput, OnEditingFinished), callback);
        Bind(fieldW, MEMBER_FUNCTION(VectorComponentInput, OnEditingFinished), callback);

        Delegate<void(CTextInput*)> editingCallback = [this, field, instance](CTextInput*)
            {
                //emit OnValueModified(this);
                if (field && instance)
                {
                    Vec4 vec4Value = GetVectorValue();

                    TypeId declId = field->GetDeclarationTypeId();

                    if (declId == TYPEID(Vec2))
                    {
                        field->SetFieldValue<Vec2>(instance, vec4Value);
                    }
                    else if (declId == TYPEID(Vec3))
                    {
                        field->SetFieldValue<Vec3>(instance, vec4Value);
                    }
                    else if (declId == TYPEID(Vec4))
                    {
                        field->SetFieldValue<Vec4>(instance, vec4Value);
                    }
                    else if (declId == TYPEID(Vec2i))
                    {
                        field->SetFieldValue<Vec2i>(instance, vec4Value.ToVec4i());
                    }
                    else if (declId == TYPEID(Vec3i))
                    {
                        field->SetFieldValue<Vec3i>(instance, vec4Value.ToVec4i());
                    }
                    else if (declId == TYPEID(Vec4i))
                    {
                        field->SetFieldValue<Vec4i>(instance, vec4Value.ToVec4i());
                    }
                }
            };

        Bind(fieldX, MEMBER_FUNCTION(VectorComponentInput, OnTextEdited), editingCallback);
        Bind(fieldY, MEMBER_FUNCTION(VectorComponentInput, OnTextEdited), editingCallback);
        Bind(fieldZ, MEMBER_FUNCTION(VectorComponentInput, OnTextEdited), editingCallback);
        Bind(fieldW, MEMBER_FUNCTION(VectorComponentInput, OnTextEdited), editingCallback);
    }

    void VectorFieldEditor::Construct()
    {
	    Super::Construct();

        fieldX = CreateObject<VectorComponentInput>(this, "VectorInput");
        fieldX->tagColor = Color::Red();

        fieldY = CreateObject<VectorComponentInput>(this, "VectorInput");
        fieldY->tagColor = Color::Green();

        fieldZ = CreateObject<VectorComponentInput>(this, "VectorInput");
        fieldZ->tagColor = Color::Blue();

        fieldW = CreateObject<VectorComponentInput>(this, "VectorInput");
        fieldW->tagColor = Color::White();

        SetVectorType<Vec4>();
    }

} // namespace CE::Editor
