#include "CrystalWidgets.h"

namespace CE::Editor
{
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

        Rect rect = Rect::FromSize(pos + Vec2(2.5f, 2.5f), Vec2(5.0f, size.height - 5.0f));

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

        Delegate<void(CTextInput*)> callback = [this](CTextInput*)
            {
                if (!fieldX->IsEditing() &&
                    !fieldY->IsEditing() &&
                    !fieldZ->IsEditing() &&
                    !fieldW->IsEditing())
                {
                    emit OnEditingFinished(this);
                }
            };

        Bind(fieldX, MEMBER_FUNCTION(CTextInput, OnEditingFinished), callback);
        Bind(fieldY, MEMBER_FUNCTION(CTextInput, OnEditingFinished), callback);
        Bind(fieldZ, MEMBER_FUNCTION(CTextInput, OnEditingFinished), callback);
        Bind(fieldW, MEMBER_FUNCTION(CTextInput, OnEditingFinished), callback);

        Delegate<void(CTextInput*)> editingCallback = [this](CTextInput*)
            {
                emit OnValueModified(this);
            };

        Bind(fieldX, MEMBER_FUNCTION(CTextInput, OnTextChanged), editingCallback);
        Bind(fieldY, MEMBER_FUNCTION(CTextInput, OnTextChanged), editingCallback);
        Bind(fieldZ, MEMBER_FUNCTION(CTextInput, OnTextChanged), editingCallback);
        Bind(fieldW, MEMBER_FUNCTION(CTextInput, OnTextChanged), editingCallback);

        SetVectorType<Vec4>();
    }

} // namespace CE::Editor
