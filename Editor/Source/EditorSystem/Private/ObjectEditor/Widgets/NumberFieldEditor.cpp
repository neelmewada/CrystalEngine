#include "EditorSystem.h"

namespace CE::Editor
{
	static const HashSet<TypeId> signedTypes = { TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64) };
	static const HashSet<TypeId> unsignedTypes = { TYPEID(u8), TYPEID(u16), TYPEID(u32), TYPEID(u64) };

	void NumberFieldEditor::BindField(FieldType* field, void* instance)
	{
		if (!field || !instance)
			return;

		TypeId declId = field->GetTypeId();

		if (declId == TYPEID(f32) || declId == TYPEID(f64))
			textInput->SetInputValidator(CFloatInputValidator);
		else if (signedTypes.Exists(declId))
			textInput->SetInputValidator(CIntegerInputValidator);
		else if (unsignedTypes.Exists(declId))
			textInput->SetInputValidator(CUnsignedIntegerInputValidator);
		else
			return;

		Delegate<void()> fieldUpdateCallback = [this, declId, field, instance]
			{
				if (declId == TYPEID(f32))
				{
					textInput->SetText(String::Format("{}", field->GetFieldValue<f32>(instance)));
				}
				else if (declId == TYPEID(f64))
				{
					textInput->SetText(String::Format("{}", field->GetFieldValue<f64>(instance)));
				}
				else if (declId == TYPEID(s16))
				{
					textInput->SetText(String::Format("{}", field->GetFieldValue<s16>(instance)));
				}
				else if (declId == TYPEID(u16))
				{
					textInput->SetText(String::Format("{}", field->GetFieldValue<u16>(instance)));
				}
				else if (declId == TYPEID(s8))
				{
					textInput->SetText(String::Format("{}", field->GetFieldValue<s8>(instance)));
				}
				else if (declId == TYPEID(u8))
				{
					textInput->SetText(String::Format("{}", field->GetFieldValue<u8>(instance)));
				}
				else if (declId == TYPEID(s32))
				{
					textInput->SetText(String::Format("{}", field->GetFieldValue<s32>(instance)));
				}
				else if (declId == TYPEID(u32))
				{
					textInput->SetText(String::Format("{}", field->GetFieldValue<u32>(instance)));
				}
				else if (declId == TYPEID(s64))
				{
					textInput->SetText(String::Format("{}", field->GetFieldValue<s64>(instance)));
				}
				else if (declId == TYPEID(u64))
				{
					textInput->SetText(String::Format("{}", field->GetFieldValue<u64>(instance)));
				}
			};

		// Manually update the gui text box initially
		fieldUpdateCallback();

		textInput->SetInteractable(field->IsEditAnywhere());

        Delegate<void(CTextInput*)> callback = [this, declId, field, instance](CTextInput* inputField)
            {
				if (field->IsReadOnly() || !field->IsEditAnywhere())
					return;

				f64 numberValue = 0.0f;
				if (String::TryParse(textInput->GetText(), numberValue))
				{
					if (declId == TYPEID(s32))
					{
						
					}
				}
            };

		Bind(textInput, MEMBER_FUNCTION(CTextInput, OnEditingFinished), callback);
		Bind(textInput, MEMBER_FUNCTION(CTextInput, OnTextChanged), callback);
	}

	void NumberFieldEditor::Construct()
	{
		Super::Construct();

		textInput = CreateObject<CTextInput>(this, "TextInput");
		
	}


} // namespace CE::Editor
