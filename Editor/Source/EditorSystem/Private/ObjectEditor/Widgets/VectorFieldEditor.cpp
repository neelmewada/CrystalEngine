#include "CrystalWidgets.h"

namespace CE::Editor
{
	void VectorComponentInput::Construct()
	{
		Super::Construct();

        tagWidget = CreateObject<CWidget>(this, "Tag");
        tagWidget->AddStyleClass("Tag");

        inputField = CreateObject<CTextInput>(this, "TextInput");
        inputField->SetInputValidator(CFloatInputValidator);
        inputField->SetText("0");
        inputField->SetSelectAllOnEdit(true);
	}

    VectorFieldEditor::VectorFieldEditor()
    {
	    
    }

    VectorFieldEditor::~VectorFieldEditor()
    {
	    
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
        String::TryParse(fieldX->inputField->GetText(), result.x);
        String::TryParse(fieldX->inputField->GetText(), result.y);
        String::TryParse(fieldX->inputField->GetText(), result.z);
        String::TryParse(fieldX->inputField->GetText(), result.w);
        return result;
    }

    Vec4i VectorFieldEditor::GetVectorIntValue()
    {
        Vec4i result{};
        String::TryParse(fieldX->inputField->GetText(), result.x);
        String::TryParse(fieldX->inputField->GetText(), result.y);
        String::TryParse(fieldX->inputField->GetText(), result.z);
        String::TryParse(fieldX->inputField->GetText(), result.w);
        return result;
    }

    void VectorFieldEditor::Construct()
    {
	    Super::Construct();

        fieldX = CreateObject<VectorComponentInput>(this, "VectorInput");
        fieldX->tagWidget->SetName("Red");

        fieldY = CreateObject<VectorComponentInput>(this, "VectorInput");
        fieldY->tagWidget->SetName("Green");

        fieldZ = CreateObject<VectorComponentInput>(this, "VectorInput");
        fieldZ->tagWidget->SetName("Blue");

        fieldW = CreateObject<VectorComponentInput>(this, "VectorInput");
        fieldW->tagWidget->SetName("White");

        Delegate<void(CTextInput*)> callback = [this](CTextInput*)
            {
                if (!fieldX->inputField->IsEditing() &&
                    !fieldY->inputField->IsEditing() &&
                    !fieldZ->inputField->IsEditing() &&
                    !fieldW->inputField->IsEditing())
                {
                    emit OnEditingFinished(this);
                }
            };

        Bind(fieldX->inputField, MEMBER_FUNCTION(CTextInput, OnEditingFinished), callback);
        Bind(fieldY->inputField, MEMBER_FUNCTION(CTextInput, OnEditingFinished), callback);
        Bind(fieldZ->inputField, MEMBER_FUNCTION(CTextInput, OnEditingFinished), callback);
        Bind(fieldW->inputField, MEMBER_FUNCTION(CTextInput, OnEditingFinished), callback);

        Delegate<void(CTextInput*)> editingCallback = [this](CTextInput*)
            {
                emit OnValueModified(this);
            };

        Bind(fieldX->inputField, MEMBER_FUNCTION(CTextInput, OnTextChanged), editingCallback);
        Bind(fieldY->inputField, MEMBER_FUNCTION(CTextInput, OnTextChanged), editingCallback);
        Bind(fieldZ->inputField, MEMBER_FUNCTION(CTextInput, OnTextChanged), editingCallback);
        Bind(fieldW->inputField, MEMBER_FUNCTION(CTextInput, OnTextChanged), editingCallback);

        SetVectorType<Vec4>();
    }

} // namespace CE::Editor
