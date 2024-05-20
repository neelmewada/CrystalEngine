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

	PropertyDrawer* PropertyDrawer::Create(FieldType* targetField, CWidget* outer, const String& name)
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

		return CreateObject<PropertyDrawer>(outer, name.NonEmpty() ? name : propertyDrawer->GetName().GetLastComponent(), OF_NoFlags, propertyDrawer);
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
				//editorWidget->SetVectorType(declTypeId);

				{
					if (declTypeId == TYPEID(Vec2))
					{
						editorWidget->SetVectorValue(field->GetFieldValue<Vec2>(instance));
					}
					else if (declTypeId == TYPEID(Vec3))
					{
						editorWidget->SetVectorValue(field->GetFieldValue<Vec3>(instance));
					}
					else if (declTypeId == TYPEID(Vec4))
					{
						editorWidget->SetVectorValue(field->GetFieldValue<Vec4>(instance));
					}
					else if (declTypeId == TYPEID(Vec2i))
					{
						editorWidget->SetVectorIntValue(field->GetFieldValue<Vec2i>(instance));
					}
					else if (declTypeId == TYPEID(Vec3i))
					{
						editorWidget->SetVectorIntValue(field->GetFieldValue<Vec3i>(instance));
					}
					else if (declTypeId == TYPEID(Vec4i))
					{
						editorWidget->SetVectorIntValue(field->GetFieldValue<Vec4i>(instance));
					}
				}

				Bind(editorWidget, MEMBER_FUNCTION(VectorFieldEditor, OnValueModified), 
					[declTypeId, field, instance, this](VectorFieldEditor* editor)
					{
						if (declTypeId == TYPEID(Vec2) || declTypeId == TYPEID(Vec3) || declTypeId == TYPEID(Vec4))
						{
							Vec4 vec4Value = editor->GetVectorValue();
							if (declTypeId == TYPEID(Vec2))
								field->SetFieldValue<Vec2>(instance, vec4Value);
							else if (declTypeId == TYPEID(Vec3))
								field->SetFieldValue<Vec3>(instance, vec4Value);
							else if (declTypeId == TYPEID(Vec4))
								field->SetFieldValue<Vec4>(instance, vec4Value);

							emit OnPropertyModified(this);
						}
						else
						{
							Vec4i vec4Value = editor->GetVectorIntValue();
							if (declTypeId == TYPEID(Vec2i))
								field->SetFieldValue<Vec2i>(instance, vec4Value);
							else if (declTypeId == TYPEID(Vec3i))
								field->SetFieldValue<Vec3i>(instance, vec4Value);
							else if (declTypeId == TYPEID(Vec4i))
								field->SetFieldValue<Vec4i>(instance, vec4Value);

							emit OnPropertyModified(this);
						}
					});
			}
			else if (declTypeId == TYPEID(String))
			{
				CTextInput* stringInput = CreateObject<CTextInput>(right, "StringInput");
				stringInput->SetText(field->GetFieldValue<String>(instance));

				Bind(stringInput, MEMBER_FUNCTION(CTextInput, OnTextChanged), [field, instance](CTextInput* input)
					{
						field->SetFieldValue<String>(instance, input->GetText());
					});
			}
			else if (declTypeId == TYPEID(bool))
			{
				CCheckBox* checkBox = CreateObject<CCheckBox>(right, "BooleanInput");
				checkBox->SetChecked(field->GetFieldValue<bool>(instance));

				Bind(checkBox, MEMBER_FUNCTION(CCheckBox, OnCheckChanged), [field, instance](CCheckBox* checkBox)
					{
						field->SetFieldValue<bool>(instance, checkBox->IsChecked());
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
