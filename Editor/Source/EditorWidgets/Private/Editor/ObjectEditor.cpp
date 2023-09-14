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
				CCollapsibleSection* collapsibleSection = CreateWidget<CCollapsibleSection>(this, "ObjectEditorCollapsible");
				collapsibleSection->SetTitle(category);

				ObjectEditorSection* section = CreateWidget<ObjectEditorSection>(collapsibleSection, "ObjectEditorSection");
				if (!section->Construct(targetType, category, targets))
				{
					collapsibleSection->Destroy(); // If section is empty
				}
			}
		}

		SetNeedsStyle();
		SetNeedsLayout();
	}

	bool ObjectEditorSection::Construct(ClassType* type, const String& category, const Array<Object*>& targets)
	{
		this->targetType = type;
		this->category = category;
		this->targets = targets;
		this->fields = {};

		for (auto label : fieldNameLabels)
			label->Destroy();
		fieldNameLabels.Clear();

		for (auto editor : fieldEditors)
			editor->Destroy();
		fieldEditors.Clear();

		Array<void*> targetInstances = this->targets.Transform<void*>([](Object* instance) { return (void*)instance; });

		DestroyAllSubWidgets();

		if (targetType == nullptr || targets.IsEmpty())
			return false;

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
			return false;
			
		if (targetType != nullptr)
		{
			for (auto field : fields)
			{
				auto fieldDeclType = field->GetDeclarationType();
				if (fieldDeclType == nullptr)
					continue;

				SubClassType<FieldEditor> fieldEditorClass = FieldEditor::GetFieldEditorClass(fieldDeclType->GetTypeName());
				if (fieldEditorClass == nullptr)
					continue;

				FieldEditor* editor = CreateWidget<FieldEditor>(this, "FieldEditor", fieldEditorClass);
				fieldEditors.Add(editor);

				Array<FieldType*> targetFields = {};
				for (int j = 0; j < targetInstances.GetSize(); j++)
					targetFields.Add(field);

				editor->SetTargets(fieldDeclType, targetFields, targetInstances);

				CLabel* label = CreateWidget<CLabel>(this, "FieldLabel");
				label->SetIndependentLayout(true);
				label->SetText(field->GetDisplayName());
				fieldNameLabels.Add(label);
			}
		}

		return true;
	}

	void ObjectEditor::OnDrawGUI()
	{
		Super::OnDrawGUI();
	}

	void ObjectEditorSection::OnAfterComputeStyle()
	{
		Super::OnAfterComputeStyle();

		auto headerSelect = stylesheet->SelectStyle(this, CStateFlag::Default, CSubControl::Header);
		LoadGuiStyleState(headerSelect, headerStyle);

		if (headerSelect.properties.KeyExists(CStylePropertyType::Padding))
		{
			headerPadding = headerSelect.properties[CStylePropertyType::Padding].vector;
		}
	}

	void ObjectEditorSection::Construct()
	{
		Super::Construct();

		tableId = GenerateRandomU32();

		LoadStyleSheet("/EditorWidgets/Resources/ObjectEditorStyle.css");
	}

	Vec2 ObjectEditorSection::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(width, totalHeight);
	}

	void ObjectEditorSection::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();

		GUI::TableFlags flags = GUI::TableFlags_BordersInner | GUI::TableFlags_Resizable;

		if (GUI::BeginTable(rect, tableId, "", 2, flags))
		{
			float height = 0;

			for (int i = 0; i < fieldNameLabels.GetSize(); i++)
			{
				float h = 0;
				CLabel* fieldLabel = fieldNameLabels[i];
				FieldEditor* fieldEditor = fieldEditors[i];
				bool isExpandable = fieldEditor->IsExpandable();
				bool isExpanded = false;

				GUI::TableNextRow();

				GUI::TableNextColumn();
				GUI::PushChildCoordinateSpace(GUI::GetCursorPos());
				{
					if (isExpandable)
					{
						GUI::PushStyleVar(GUI::StyleVar_FramePadding, Vec2(5, 5));
						GUI::PushStyleColor(GUI::StyleCol_Header, Color::Clear());
						GUI::PushStyleColor(GUI::StyleCol_HeaderActive, Color::Clear());
						GUI::PushStyleColor(GUI::StyleCol_HeaderHovered, Color::Clear());
						isExpanded = GUI::TreeNode(fieldLabel->GetText(), GUI::TNF_SpanFullWidth | GUI::TNF_FramePadding | GUI::TNF_DefaultOpen);
						GUI::PopStyleColor(3);
						GUI::PopStyleVar(1);
					}
					else
					{
						fieldLabel->Render();
					}
					h = Math::Max(h, GUI::GetItemRectSize().height);
				}
				GUI::PopChildCoordinateSpace();

				auto colWidth = GUI::TableGetColumnWidth(0);
				if (columnWidths[0] != colWidth)
				{
					SetNeedsLayout();
					fieldLabel->SetNeedsLayout();
					fieldEditor->SetNeedsLayout();
				}
				columnWidths[0] = colWidth;

				GUI::TableNextColumn();
				GUI::PushChildCoordinateSpace(GUI::GetCursorPos());
				{
					fieldEditor->Render();
					h = Math::Max(h, GUI::GetItemRectSize().height);
				}
				GUI::PopChildCoordinateSpace();

				colWidth = GUI::TableGetColumnWidth(1);
				if (columnWidths[1] != colWidth)
				{
					SetNeedsLayout();
					fieldLabel->SetNeedsLayout();
					fieldEditor->SetNeedsLayout();
				}
				columnWidths[1] = colWidth;

				height += h + 1;

				if (isExpanded)
				{
					height += fieldEditor->RenderExpansion();

					GUI::TreePop();
				}
			}

			if (height != totalHeight)
			{
				totalHeight = height;
				SetNeedsLayout();
			}

			GUI::EndTable();
		}
	}

} // namespace CE::Editor
