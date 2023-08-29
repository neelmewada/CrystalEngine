#include "EditorWidgets.h"

namespace CE::Editor
{
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
		ClassType* type = nullptr;

		for (auto target : targets)
		{
			if (target == nullptr)
				return false;
			if (type == nullptr)
			{
				type = target->GetClass();
			}
			else if (type->IsSubclassOf(target->GetClass()))
			{
				type = target->GetClass();
			}
			else if (target->GetClass()->IsSubclassOf(type))
			{
				
			}
			else
			{
				return false;
			}
		}

		this->targets.Clear();

		if (type == nullptr)
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

		this->targetType = type;

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

		DestroyAllSubWidgets();
		ConstructWidgets();

		return this->targets.NonEmpty();
	}

	Vec2 ObjectEditor::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(width, (numFields + categories.GetSize()) * 15.0f);
	}

	void ObjectEditor::Construct()
	{
		Super::Construct();

		LoadStyleSheet("/EditorWidgets/Resources/ObjectEditorStyle.css");

		ConstructWidgets();
	}

	void ObjectEditor::ConstructWidgets()
	{
		for (const auto& category : categories)
		{
			ObjectEditorSection* section = CreateWidget<ObjectEditorSection>(this, "ObjectEditorSection");
			section->Construct(targetType, category, targets);
		}

		SetNeedsStyle();
		SetNeedsLayout();
	}

	void ObjectEditorSection::Construct(ClassType* type, const String& category, const Array<Object*>& targets)
	{
		this->targetType = type;
		this->category = category;
		this->targets = targets;

		DestroyAllSubWidgets();

		CCollapsibleSection* section = CreateWidget<CCollapsibleSection>(this, "ObjectEditorCollapsible");
		section->SetTitle(category);
		{
			CSplitView* splitView = CreateWidget<CSplitView>(section, "EditorSplitView");
			auto splitViewLeft = splitView->GetLeft();
			auto splitViewRight = splitView->GetRight();

			for (auto field = targetType->GetFirstField(); field != nullptr; field = field->GetNext())
			{
				if ((category == "General" && !field->HasAttribute("Category")) ||
					(field->HasAttribute("Category") && field->GetAttribute("Category").GetStringValue() == category))
				{
					CLabel* label = CreateWidget<CLabel>(splitViewLeft, "FieldLabel");
					label->SetText(field->GetDisplayName());

					CLabel* label2 = CreateWidget<CLabel>(splitViewRight, "FieldValue");
					label2->SetText("Field value here");
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
