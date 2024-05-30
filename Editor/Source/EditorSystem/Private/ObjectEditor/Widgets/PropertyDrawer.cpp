#include "EditorSystem.h"

namespace CE::Editor
{
	HashMap<Pair<TypeId, TypeId>, Array<SubClass<PropertyDrawer>>> PropertyDrawer::customProperyDrawers{};

	PropertyDrawer::PropertyDrawer()
	{
		
	}

	PropertyDrawer::~PropertyDrawer()
	{
		
	}

	void PropertyDrawer::RegisterPropertyDrawer(TypeId fieldDeclTypeId, SubClass<PropertyDrawer> propertyDrawer)
	{
		RegisterPropertyDrawer(fieldDeclTypeId, 0, propertyDrawer);
	}

	void PropertyDrawer::RegisterPropertyDrawer(TypeId fieldDeclTypeId, TypeId underlyingTypeId, SubClass<PropertyDrawer> propertyDrawer)
	{
		if (propertyDrawer == nullptr)
			return;

		customProperyDrawers[Pair(fieldDeclTypeId, underlyingTypeId)].Add(propertyDrawer);
	}

	void PropertyDrawer::DeregisterPropertyDrawer(TypeId fieldDeclTypeId, SubClass<PropertyDrawer> propertyDrawer)
	{
		DeregisterPropertyDrawer(fieldDeclTypeId, 0, propertyDrawer);
	}

	void PropertyDrawer::DeregisterPropertyDrawer(TypeId fieldDeclTypeId, TypeId underlyingTypeId, SubClass<PropertyDrawer> propertyDrawer)
	{
		if (propertyDrawer == nullptr)
			return;

		customProperyDrawers[Pair(fieldDeclTypeId, underlyingTypeId)].Remove(propertyDrawer);
	}

	PropertyDrawer* PropertyDrawer::Create(ObjectEditor* owner, FieldType* targetField, CWidget* outer, const String& name)
	{
		if (!targetField || !outer)
			return nullptr;

		TypeId declTypeId = targetField->GetDeclarationTypeId();
		TypeId underlyingTypeId = targetField->GetUnderlyingTypeId();

		if (declTypeId == 0)
			return nullptr;

		TypeInfo* typeInfo = GetTypeInfo(declTypeId);
		if (!typeInfo)
			return nullptr;

		SubClass<PropertyDrawer> propertyDrawer = nullptr;

		if (typeInfo->IsClass() || typeInfo->IsStruct())
		{
			Struct* structType = (Struct*)typeInfo;
			underlyingTypeId = 0;

			while (structType != nullptr && propertyDrawer == nullptr)
			{
				declTypeId = structType->GetTypeId();

				if (customProperyDrawers.KeyExists(Pair(declTypeId, underlyingTypeId)) &&
					customProperyDrawers[Pair(declTypeId, underlyingTypeId)].NonEmpty())
				{
					propertyDrawer = customProperyDrawers[Pair(declTypeId, underlyingTypeId)].Top();
				}

				if (structType->GetSuperTypesCount() == 0)
					break;

				TypeId structTypeId = structType->GetSuperType(0);
				structType = StructType::FindStruct(structTypeId);
			}
		}
		else
		{
			if (customProperyDrawers.KeyExists(Pair(declTypeId, underlyingTypeId)) &&
				customProperyDrawers[Pair(declTypeId, underlyingTypeId)].NonEmpty())
			{
				propertyDrawer = customProperyDrawers[Pair(declTypeId, underlyingTypeId)].Top();
			}
		}

		if (propertyDrawer == nullptr)
			propertyDrawer = GetStaticClass<PropertyDrawer>();

		PropertyDrawer* result = CreateObject<PropertyDrawer>(outer, name.NonEmpty() ? name : propertyDrawer->GetName().GetLastComponent(), OF_NoFlags, propertyDrawer);
		result->owner = owner;
		return result;
	}

	void PropertyDrawer::CreateGUI(FieldType* field, void* instance)
	{
		if (field == nullptr || instance == nullptr)
			return;

		CLabel* fieldLabel = CreateObject<CLabel>(left, "FieldLabel");
		fieldLabel->SetText(field->GetDisplayName());

		TypeId declTypeId = field->GetDeclarationTypeId();
		TypeId underlyingTypeId = field->GetUnderlyingTypeId();

		TypeInfo* declType = GetTypeInfo(declTypeId);

		if (!declType)
			return;

		targetField = field;
		targetInstance = instance;

		// TODO: Create editor widget

		if (field->IsObjectField())
		{
			
		}
		else if (field->IsStructField())
		{
			
		}
		else if (field->IsEnumField())
		{
			EnumFieldEditor* enumInput = CreateObject<EnumFieldEditor>(right, "EnumFieldEditor");
			enumInput->BindField(field, instance);
		}
		else if (field->IsArrayField())
		{
			
		}
		else
		{
			if (declTypeId == TYPEID(Vec2) || declTypeId == TYPEID(Vec2i) ||
				declTypeId == TYPEID(Vec3) || declTypeId == TYPEID(Vec3i) ||
				declTypeId == TYPEID(Vec4) || declTypeId == TYPEID(Vec4i))
			{
				VectorFieldEditor* editorWidget = CreateObject<VectorFieldEditor>(right, "VectorFieldEditor");
				editorWidget->SetVectorType(declTypeId);

				editorWidget->BindField(field, instance);
			}
			else if (declTypeId == TYPEID(String))
			{
				CTextInput* stringInput = CreateObject<CTextInput>(right, "StringInput");
				stringInput->SetText(field->GetFieldValue<String>(instance));

				Bind(stringInput, MEMBER_FUNCTION(CTextInput, OnTextEdited), [field, instance](CTextInput* input)
					{
						field->SetFieldValue<String>(instance, input->GetText());

						if (field->GetInstanceOwnerType()->IsClass())
						{
							((Object*)instance)->OnFieldEdited(field);
						}
					});
			}
			else if (declTypeId == TYPEID(bool))
			{
				CCheckBox* checkBox = CreateObject<CCheckBox>(right, "BooleanInput");
				checkBox->SetChecked(field->GetFieldValue<bool>(instance));

				Bind(checkBox, MEMBER_FUNCTION(CCheckBox, OnCheckChanged), [field, instance](CCheckBox* checkBox)
					{
						field->SetFieldValue<bool>(instance, checkBox->IsChecked());

						if (field->GetInstanceOwnerType()->IsClass())
						{
							((Object*)instance)->OnFieldEdited(field);
						}
					});
			}
			else if (declTypeId == TYPEID(Color))
			{
				ColorFieldEditor* colorWidget = CreateObject<ColorFieldEditor>(right, "ColorFieldEditor");

				colorWidget->BindField(field, instance);
			}
			else if (field->IsDecimalField() || field->IsNumberField())
			{
				NumericFieldInput* numberInput = CreateObject<NumericFieldInput>(right, "NumberInput");

				if (field->IsDecimalField())
				{
					if (declTypeId == TYPEID(f32))
						numberInput->SetPrecision(floatPrecision);
					else
						numberInput->SetPrecision(doublePrecision);

					numberInput->SetText(String::Format("{}", field->GetNumericFieldValue(instance)));
				}
				else
				{
					if (declTypeId == TYPEID(u64))
						numberInput->SetText(String::Format("{}", (u64)field->GetNumericFieldValue(instance)));
					else
						numberInput->SetText(String::Format("{}", (s64)field->GetNumericFieldValue(instance)));
				}

				Bind(numberInput, MEMBER_FUNCTION(NumericFieldInput, OnTextEdited), [field, instance, declTypeId](CTextInput* textInput)
					{
						const String& text = textInput->GetText();
						f64 numberValue = 0.0f;

						if (String::TryParse(text, numberValue))
						{
							if (declTypeId == TYPEID(f32))
							{
								field->SetFieldValue<f32>(instance, (f32)numberValue);
							}
							else if (declTypeId == TYPEID(f64))
							{
								field->SetFieldValue<f64>(instance, numberValue);
							}
							else if (declTypeId == TYPEID(u32))
							{
								field->SetFieldValue<u32>(instance, (u32)numberValue);
							}
							else if (declTypeId == TYPEID(s32))
							{
								field->SetFieldValue<s32>(instance, (s32)numberValue);
							}
							else if (declTypeId == TYPEID(u64))
							{
								field->SetFieldValue<u64>(instance, (u64)numberValue);
							}
							else if (declTypeId == TYPEID(s64))
							{
								field->SetFieldValue<s64>(instance, (s64)numberValue);
							}
							else if (declTypeId == TYPEID(u16))
							{
								field->SetFieldValue<u16>(instance, (u16)numberValue);
							}
							else if (declTypeId == TYPEID(s16))
							{
								field->SetFieldValue<s16>(instance, (s16)numberValue);
							}
							else if (declTypeId == TYPEID(u8))
							{
								field->SetFieldValue<u8>(instance, (u8)numberValue);
							}
							else if (declTypeId == TYPEID(s8))
							{
								field->SetFieldValue<s8>(instance, (s8)numberValue);
							}

							if (field->GetInstanceOwnerType()->IsClass())
							{
								((Object*)instance)->OnFieldEdited(field);
							}
						}
					});
			}
		}
	}

	void PropertyDrawer::Construct()
	{
		Super::Construct();

		left = CreateObject<CWidget>(this, "PropertyEditorRowLeft");
		
		right = CreateObject<CWidget>(this, "PropertyEditorRowRight");

	}

} // namespace CE::Editor
