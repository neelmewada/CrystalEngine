#include "EditorCore.h"

namespace CE::Editor
{
	HashMap<Name, Array<ClassType*>> ObjectEditor::classTypeToEditorMap{};

	ObjectEditor::ObjectEditor()
	{
	}

	ObjectEditor::~ObjectEditor()
	{
	}

	ObjectEditorSection::ObjectEditorSection()
	{
	}

	ObjectEditorSection::~ObjectEditorSection()
	{
	}

	bool ObjectEditor::SetTargets(const Array<Object*>& targets)
	{
		ClassType* editedType = nullptr;

		for (auto target : targets)
		{
			if (target == nullptr)
				return false;
			if (editedType == nullptr)
			{
				editedType = target->GetClass();
			}
			else if (editedType->IsSubclassOf(target->GetClass()))
			{
				editedType = target->GetClass();
			}
			else if (target->GetClass()->IsSubclassOf(editedType))
			{
				// Do nothing
			}
			else
			{
				// Different target type: target is out of inheritence hierarchy
				return false;
			}
		}

		this->targets.Clear();

		if (editedType == nullptr)
		{
			return false;
		}

		for (auto target : targets)
		{
			if (target->GetClass()->IsSubclassOf(GetTargetClassType()))
			{
				this->targets.Add(target);
			}
		}

		this->targetType = editedType;

		categories.Add("General");
		numFields = targetType->GetFieldCount();

		for (auto field = targetType->GetFirstField(); field != nullptr; field = field->GetNext())
		{
			if (field->HasAttribute("Category"))
			{
				String category = field->GetAttribute("Category");
				if (!categories.Exists(category))
					categories.Add(category);
			}
		}

		ConstructWidgets();

		return this->targets.NonEmpty();
	}

	Vec2 ObjectEditor::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(width, (numFields + categories.GetSize()) * 15.0f);
	}

	SubClassType<ObjectEditor> ObjectEditor::GetObjectEditorClass(const Name& objectTypeName)
	{
		auto classType = ClassType::FindClass(objectTypeName);
		if (classType == nullptr)
			return ObjectEditor::StaticType();

		auto baseClass = classType;
		while (baseClass != nullptr)
		{
			if (classTypeToEditorMap.KeyExists(baseClass->GetTypeName()))
			{
				const auto& array = classTypeToEditorMap[baseClass->GetTypeName()];
				if (array.NonEmpty())
					return array.Top();
			}

			if (baseClass->GetSuperClassCount() > 0)
				baseClass = baseClass->GetSuperClass(0);
			else break;
		}

		return ObjectEditor::StaticType();
	}

	ObjectEditor* ObjectEditor::CreateEditorFor(const Name& targetObjectTypeName, CWidget* parent)
	{
		ClassType* objectEditorClass = GetObjectEditorClass(targetObjectTypeName);
		if (objectEditorClass == nullptr)
			return nullptr;

		return CreateWidget<ObjectEditor>(parent, "ObjectEditor", objectEditorClass);
	}

	void ObjectEditor::Construct()
	{
		Super::Construct();

		LoadStyleSheet("/EditorWidgets/Resources/ObjectEditorStyle.css");

		ConstructWidgets();
	}

	void ObjectEditor::ConstructWidgets()
	{
		DestroyAllSubWidgets();

		if (targetType != nullptr && categories.NonEmpty() && targets.NonEmpty())
		{
			for (const auto& category : categories)
			{
				ObjectEditorSection* section = CreateWidget<ObjectEditorSection>(this, "ObjectEditorSection");
				section->Construct(targetType, category, targets);
			}
		}

		SetNeedsStyle();
		SetNeedsLayout();
	}

	void ObjectEditorSection::Construct(ClassType* type, const String& category, const Array<Object*>& targets)
	{
		this->targetType = type;
		this->category = category;
		this->targets = targets;
		Array<void*> targetInstances = this->targets.Transform<void*>([](Object* instance) { return (void*)instance; });

		DestroyAllSubWidgets();

		if (targetType == nullptr || targets.IsEmpty())
			return;

		Array<FieldType*> fields = {};

		for (auto field = targetType->GetFirstField(); field != nullptr; field = field->GetNext())
		{
			if (field->IsHidden())
				continue;
			if ((category == "General" && !field->HasAttribute("Category")) ||
				(field->HasAttribute("Category") && field->GetAttribute("Category").GetStringValue() == category))
			{
				fields.Add(field);
			}
		}

		if (fields.IsEmpty())
			return;

		CCollapsibleSection* section = CreateWidget<CCollapsibleSection>(this, "ObjectEditorCollapsible");
		section->SetTitle(category);
		{
			CTableWidget* table = CreateWidget<CTableWidget>(section, "ObjectEditorTableWidget");
			table->SetTableFlags(CTableFlags::InnerBorders | CTableFlags::ResizeableColumns);
			table->SetNumColumns(2);
			
			if (targetType != nullptr)
			{
				table->SetNumRows(fields.GetSize());
				int i = 0;

				for (auto field : fields)
				{
					auto fieldDeclType = field->GetDeclarationType();
					if (fieldDeclType == nullptr)
						continue;

					SubClassType<FieldEditor> fieldEditorClass = FieldEditor::GetFieldEditorClass(fieldDeclType->GetTypeName());
					if (fieldEditorClass == nullptr)
						continue;

					CLabel* label = CreateWidget<CLabel>(table->GetCellWidget(Vec2i(i, 0)), "FieldLabel");
					label->SetText(field->GetDisplayName());

					//CLabel* value = CreateWidget<CLabel>(table->GetCellWidget(Vec2i(i, 1)), "FieldEditor");
					//value->SetText("Field value here");
					FieldEditor* editor = CreateWidget<FieldEditor>(table->GetCellWidget(Vec2i(i, 1)), "FieldEditor", fieldEditorClass);

					Array<FieldType*> targetFields = {};
					for (int j = 0; j < targetInstances.GetSize(); j++)
						targetFields.Add(field);

					editor->SetTargets(fieldDeclType, targetFields, targetInstances);

					i++;
				}
			}
		}
	}

	void ObjectEditor::OnDrawGUI()
	{
		Super::OnDrawGUI();
	}

	void ObjectEditorSection::Construct()
	{
		Super::Construct();

		LoadStyleSheet("/EditorWidgets/Resources/ObjectEditorStyle.css");
	}

	void ObjectEditorSection::OnDrawGUI()
	{
		Super::OnDrawGUI();
	}

} // namespace CE::Editor
