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

				emit OnValueUpdated();
			};

		// Manually update the gui text box initially
		fieldUpdateCallback();

		textInput->SetInteractable(field->IsEditAnywhere());

        Delegate<void(CTextInput*)> callback = [this, declId, field, instance](CTextInput*)
            {
				if (field->IsReadOnly() || !field->IsEditAnywhere())
					return;

				f64 numberValue = 0.0f;
				if (String::TryParse(textInput->GetText(), numberValue))
				{
					if (declId == TYPEID(s32))
					{
						field->SetFieldValue<s32>(instance, (s32)numberValue);
					}
					else if (declId == TYPEID(u32))
					{
						field->SetFieldValue<u32>(instance, (u32)numberValue);
					}
					else if (declId == TYPEID(s64))
					{
						field->SetFieldValue<s64>(instance, (s64)numberValue);
					}
					else if (declId == TYPEID(u64))
					{
						field->SetFieldValue<u64>(instance, (u64)numberValue);
					}
					else if (declId == TYPEID(s16))
					{
						field->SetFieldValue<s16>(instance, (s16)numberValue);
					}
					else if (declId == TYPEID(u16))
					{
						field->SetFieldValue<u16>(instance, (u16)numberValue);
					}
					else if (declId == TYPEID(s8))
					{
						field->SetFieldValue<s8>(instance, (s8)numberValue);
					}
					else if (declId == TYPEID(u8))
					{
						field->SetFieldValue<u8>(instance, (u8)numberValue);
					}
					else if (declId == TYPEID(f32))
					{
						field->SetFieldValue<f32>(instance, (f32)numberValue);
					}
					else if (declId == TYPEID(f64))
					{
						field->SetFieldValue<f64>(instance, numberValue);
					}

					emit OnValueUpdated();
				}
            };

		Bind(textInput, MEMBER_FUNCTION(CTextInput, OnEditingFinished), callback);
		Bind(textInput, MEMBER_FUNCTION(CTextInput, OnTextEdited), callback);
	}

	void NumberFieldEditor::Construct()
	{
		Super::Construct();

		textInput = CreateObject<CTextInput>(this, "TextInput");
		
	}


} // namespace CE::Editor
