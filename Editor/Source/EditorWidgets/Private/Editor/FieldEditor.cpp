#include "EditorWidgets.h"

namespace CE::Editor
{
	HashMap<Name, Array<ClassType*>> FieldEditor::editorClassesByFieldType{};

	static const Array<TypeId> textTypes = {
		TYPEID(String), TYPEID(Name),
		TYPEID(UUID), TYPEID(UUID32),
		TYPEID(c8), TYPEID(c16),
		TYPEID(u8), TYPEID(u16), TYPEID(u32), TYPEID(u64),
		TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64),
		TYPEID(f32), TYPEID(f64)
	};

    FieldEditor::FieldEditor()
    {

    }

    FieldEditor::~FieldEditor()
    {

    }

	SubClassType<FieldEditor> FieldEditor::GetFieldEditorClass(Name fieldDeclarationTypeName)
	{
		TypeInfo* fieldDeclType = GetTypeInfo(fieldDeclarationTypeName);
		if (fieldDeclType == nullptr)
			return FieldEditor::StaticType();

		if (fieldDeclType->IsClass())
		{
			ClassType* classType = (ClassType*)fieldDeclType;
			ClassType* baseClass = classType;

			while (baseClass != nullptr)
			{
				if (editorClassesByFieldType.KeyExists(baseClass->GetTypeName()))
				{
					const auto& array = editorClassesByFieldType[baseClass->GetTypeName()];
					if (array.NonEmpty())
						return array.Top();
				}

				if (baseClass->GetSuperClassCount() > 0)
					baseClass = baseClass->GetSuperClass(0);
				else break;
			}
		}
		else if (fieldDeclType->IsStruct())
		{
			StructType* structType = (StructType*)fieldDeclType;
			
			if (editorClassesByFieldType.KeyExists(structType->GetTypeName()))
			{
				const auto& array = editorClassesByFieldType[structType->GetTypeName()];
				if (array.NonEmpty())
					return array.Top();
			}
		}
		else if (editorClassesByFieldType.KeyExists(fieldDeclarationTypeName))
		{
			return editorClassesByFieldType[fieldDeclarationTypeName].Top();
		}

		return FieldEditor::StaticType();
	}

	bool FieldEditor::IsFieldTypeSupported(TypeInfo* type)
	{
		if (type == nullptr)
			return false;

		auto typeId = type->GetTypeId();
		
		if (type->IsStruct() ||
			type->IsObject() ||
			type->IsEnum())
		{
			return true;
		}

		static const Array<TypeId> allowedTypeIds = {
			TYPEID(Vec2), TYPEID(Vec3), TYPEID(Vec4),
			TYPEID(Vec2i), TYPEID(Vec3i), TYPEID(Vec4i),
			TYPEID(Quat), TYPEID(Color), TYPEID(Gradient),
			TYPEID(String), TYPEID(Name),
			TYPEID(UUID), TYPEID(UUID32), TYPEID(Array<u8>),
			TYPEID(b8), TYPEID(c8), TYPEID(c16),
			TYPEID(u8), TYPEID(u16), TYPEID(u32), TYPEID(u64),
			TYPEID(s8), TYPEID(s16), TYPEID(s32), TYPEID(s64),
			TYPEID(f32), TYPEID(f64)
		};

		return allowedTypeIds.Exists(typeId);
	}

	bool FieldEditor::SetTargets(TypeInfo* fieldDeclType, const Array<FieldType*>& fieldTypes, const Array<void*>& targetInstances)
	{
		this->fieldDeclType = nullptr;
		this->fieldTypes = {};
		this->targets = {};

		textValidator = nullptr;
		String validator = "";

		for (auto field : fieldTypes)
		{
			if (!field->IsAssignableTo(fieldDeclType->GetTypeId()))
				return false;

			if (field->HasAttribute(FIELD_ATTRIB_VALIDATOR))
			{
				String validatorType = field->GetAttribute(FIELD_ATTRIB_VALIDATOR).GetStringValue();
				if (textValidator.IsValid() && !validatorType.IsEmpty() && validator != validatorType) // Input validator mismatch
				{
					return false;
				}

				if (!validatorType.IsEmpty())
				{
					validator = validatorType;
					if (validatorType == FIELD_ATTRIB_NAME_VALIDATOR)
					{
						textValidator = CNameInputValidator;
					}
					else if (validatorType == FIELD_ATTRIB_FLOAT_VALIDATOR)
					{
						textValidator = CFloatInputValidator;
					}
					else if (validatorType == FIELD_ATTRIB_INTEGER_VALIDATOR)
					{
						textValidator = CIntegerInputValidator;
					}
					else if (validatorType == FIELD_ATTRIB_UNSIGNED_INTEGER_VALIDATOR)
					{
						textValidator = CUnsignedIntegerInputValidator;
					}
					else if (validatorType == FIELD_ATTRIB_VERSION_VALIDATOR)
					{
						textValidator = CVersionInputValidator;
					}
					else if (validatorType == FIELD_ATTRIB_LONG_VERSION_VALIDATOR)
					{
						textValidator = CLongVersionInputValidator;
					}
					else if (validatorType == FIELD_ATTRIB_NO_VALIDATOR)
					{
						textValidator = nullptr;
					}
				}
			}
		}

		if (fieldTypes.GetSize() != targetInstances.GetSize())
		{
			textValidator = nullptr;
			return false;
		}

		this->fieldDeclType = fieldDeclType;
		this->fieldTypes = fieldTypes;
		this->targets = targetInstances;

		Construct();
		return true;
	}

	bool FieldEditor::IsExpandable()
	{
		if (fieldDeclType != nullptr)
		{
			if (fieldDeclType->IsArrayType())
				return true;
		}
		return false;
	}

	float FieldEditor::RenderExpansion()
	{
		if (!IsExpandable() || fieldDeclType == nullptr)
			return 0;

		bool needsLayout = NeedsLayout();
		UpdateStyleIfNeeded();
		UpdateLayoutIfNeeded();

		float height = 0;

		if (IsVisible())
		{
			if (fieldDeclType->IsArrayType())
			{
				while (childrenEditors.GetSize() > targets.GetSize()) // Delete extra children editors
				{
					childrenEditors.GetLast()->Destroy();
					childrenEditors.RemoveAt(childrenEditors.GetSize() - 1);
					childrenLabels.GetLast()->Destroy();
					childrenLabels.RemoveAt(childrenLabels.GetSize() - 1);
				}

				bool match = true;
				int arraySize = 0;

				for (int i = 0; i < fieldTypes.GetSize(); i++)
				{
					auto curArraySize = fieldTypes[i]->GetArraySize(targets[i]);
					if (i == 0)
					{
						arraySize = curArraySize;
					}
					else if (curArraySize != arraySize)
					{
						match = false;
						break;
					}
					else
					{
						arraySize = curArraySize;
					}
				}

				isIncompatible = !match;

				if (!match)
				{
					GUI::TableNextRow();

					GUI::TableNextColumn();
					GUI::Text("");
					float h = GUI::GetItemRectSize().height;

					GUI::TableNextColumn();
					GUI::Text("[Incompatible Arrays]");
					h = Math::Max(h, GUI::GetItemRectSize().height);

					return h + 1;
				}

				if (childrenArrayFields.GetSize() != arraySize)
				{
					// TODO: set fields
				}

				for (int i = 0; i < Math::Max(childrenEditors.GetSize(), targets.GetSize()); i++)
				{
					CLabel* label = nullptr;
					FieldEditor* fieldEditor = nullptr;

					if (i >= childrenEditors.GetSize()) // Add new children editors
					{
						auto fieldEditorClass = FieldEditor::GetFieldEditorClass(fieldDeclType->GetTypeName());
						if (fieldEditorClass == nullptr)
							continue;
						fieldEditor = CreateWidget<FieldEditor>(this, "FieldEditor", fieldEditorClass);
						if (fieldEditor == nullptr)
							continue;
						
						//fieldEditor->SetTargets(fieldDeclType, fieldT)
						childrenEditors.Add(fieldEditor);
						label = CreateWidget<CLabel>(this, "FieldLabel");
						label->SetText(String::Format("Element {}", i));
						childrenLabels.Add(label);
					}
					else
					{
						label = childrenLabels[i];
						fieldEditor = childrenEditors[i];
					}

					if (needsLayout)
					{
						label->SetNeedsLayout();
						fieldEditor->SetNeedsLayout();
					}

					GUI::TableNextRow();

					GUI::TableNextColumn();
					label->Render();
					float h = label->GetComputedLayoutSize().height;

					GUI::TableNextColumn();
					fieldEditor->Render();
					h = Math::Max(h, fieldEditor->GetComputedLayoutSize().height);

					height += h;
				}
			}
		}

		return height;
	}

	Vec2 FieldEditor::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(120, 18);
	}

	void FieldEditor::Construct()
	{
		Super::Construct();

		DestroyAllSubWidgets();

		if (fieldDeclType == nullptr || targets.IsEmpty() || fieldTypes.IsEmpty() ||
			(fieldTypes.GetSize() != targets.GetSize()))
		{
			auto label = CreateWidget<CLabel>(this, "FieldIncompatibleLabel");
			label->SetText("Incompatible values");
			return;
		}

		if (textTypes.Exists(fieldDeclType->GetTypeId()))
		{
			auto input = CreateWidget<CTextInput>(this, "FieldTextInput");
			input->SetInputValidator(textValidator);

			String value = "";

			for (int i = 0; i < fieldTypes.GetSize(); i++)
			{
				if (i == 0)
				{
					value = fieldTypes[i]->GetFieldValueAsString(targets[i]);
				}
				else
				{
					String str = fieldTypes[i]->GetFieldValueAsString(targets[i]);
					if (value != str)
					{
						value = "[Multiple]";
						break;
					}
				}
			}
			
			input->SetText(value);
			auto fieldTypeId = fieldDeclType->GetTypeId();

			if (fieldTypeId == TYPEID(Name))
			{
				input->SetInputValidator(CNameInputValidator);
			}

			if (fieldTypeId == TYPEID(u8) ||
				fieldTypeId == TYPEID(u16) ||
				fieldTypeId == TYPEID(u32) ||
				fieldTypeId == TYPEID(u64) ||
				fieldTypeId == TYPEID(c8) ||
				fieldTypeId == TYPEID(c16))
			{
				input->SetInputValidator(CUnsignedIntegerInputValidator);
			}
			else if (fieldTypeId == TYPEID(s8) ||
				fieldTypeId == TYPEID(s16) ||
				fieldTypeId == TYPEID(s32) || 
				fieldTypeId == TYPEID(s64))
			{
				input->SetInputValidator(CIntegerInputValidator);
			}
			else if (fieldTypeId == TYPEID(f32) ||
				fieldTypeId == TYPEID(f64))
			{
				input->SetInputValidator(CFloatInputValidator);
			}

			Object::Bind(input, MEMBER_FUNCTION(CTextInput, OnTextEdited), [=](String newText)
				{
					if (fieldTypeId == TYPEID(c8))
					{
						c8 value = 0;
						if (String::TryParse(newText, value))
							SetTargetFieldValue(value);
					}
					else if (fieldTypeId == TYPEID(c16))
					{
						c16 value = 0;
						if (String::TryParse(newText, value))
							SetTargetFieldValue(value);
					}
					else if (fieldTypeId == TYPEID(u8))
					{
						u8 value = 0;
						if (String::TryParse(newText, value))
							SetTargetFieldValue(value);
					}
					else if (fieldTypeId == TYPEID(u16))
					{
						u16 value = 0;
						if (String::TryParse(newText, value))
							SetTargetFieldValue(value);
					}
					else if (fieldTypeId == TYPEID(u32))
					{
						u32 value = 0;
						if (String::TryParse(newText, value))
							SetTargetFieldValue(value);
					}
					else if (fieldTypeId == TYPEID(u64))
					{
						u64 value = 0;
						if (String::TryParse(newText, value))
							SetTargetFieldValue(value);
					}
					else if (fieldTypeId == TYPEID(s8))
					{
						s8 value = 0;
						if (String::TryParse(newText, value))
							SetTargetFieldValue(value);
					}
					else if (fieldTypeId == TYPEID(s16))
					{
						s16 value = 0;
						if (String::TryParse(newText, value))
							SetTargetFieldValue(value);
					}
					else if (fieldTypeId == TYPEID(s32))
					{
						s32 value = 0;
						if (String::TryParse(newText, value))
							SetTargetFieldValue(value);
					}
					else if (fieldTypeId == TYPEID(s64))
					{
						s64 value = 0;
						if (String::TryParse(newText, value))
							SetTargetFieldValue(value);
					}
					else if (fieldTypeId == TYPEID(f32))
					{
						f32 value = 0;
						if (String::TryParse(newText, value))
							SetTargetFieldValue(value);
					}
					else if (fieldTypeId == TYPEID(f64))
					{
						f64 value = 0;
						if (String::TryParse(newText, value))
							SetTargetFieldValue(value);
					}
					else if (fieldTypeId == TYPEID(UUID))
					{
						u64 value = 0;
						if (String::TryParse(newText, value))
							SetTargetFieldValue<UUID>(value);
					}
					else if (fieldTypeId == TYPEID(UUID32))
					{
						u32 value = 0;
						if (String::TryParse(newText, value))
							SetTargetFieldValue<UUID32>(value);
					}
					else if (fieldTypeId == TYPEID(String))
					{
						SetTargetFieldValue(newText);
					}
					else if (fieldTypeId == TYPEID(Name))
					{
						SetTargetFieldValue<Name>(newText);
					}
				});
		}
		else if (fieldDeclType->GetTypeId() == TYPEID(b8)) // Bool
		{
			auto checkbox = CreateWidget<CCheckbox>(this, "FieldCheckbox");
			s8 value = 0;

			for (int i = 0; i < fieldTypes.GetSize(); i++)
			{
				if (i == 0)
				{
					value = fieldTypes[i]->GetFieldValue<b8>(targets[i]) ? 1 : 0;
				}
				else
				{
					b8 fieldValue = fieldTypes[i]->GetFieldValue<b8>(targets[i]);
					if ((value > 0) != fieldValue)
					{
						value = -1; // Multiple
						break;
					}
				}
			}

			checkbox->SetRawValue(value);

			Object::Bind(checkbox, MEMBER_FUNCTION(CCheckbox, OnValueChanged), [=](s8 value)
				{
					auto tagsAndLayers = GetSettings<TagAndLayerSettings>();
					SetTargetFieldValue<b8>(value > 0);
					String::Format("");
				});
		}
		else if (fieldDeclType->IsArrayType()) // Array type
		{
			auto button = CreateWidget<CButton>(this, "AddButton");
			button->LoadIcon("Icons/add.png");
			button->SetIconSize(18);
			button->SetText("");
			button->AddStyleClass("IconButton");

			Object::Bind(button, MEMBER_FUNCTION(CButton, OnButtonClicked), [=]
				{
					for (int i = 0; i < fieldTypes.GetSize(); i++)
					{
						if (!fieldTypes[i]->IsArrayField())
							continue;

						fieldTypes[i]->InsertArrayElement(targets[i]);
					}

					SetNeedsStyle();
					SetNeedsLayout();
				});
		}

		SetNeedsStyle();
		SetNeedsLayout();
	}

	void FieldEditor::OnDrawGUI()
	{
		Super::OnDrawGUI();
	}

} // namespace CE::Editor
