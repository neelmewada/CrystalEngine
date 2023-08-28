#include "EditorWidgets.h"

namespace CE::Editor
{
	ObjectEditor::ObjectEditor()
	{

	}

	ObjectEditor::~ObjectEditor()
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

		RemoveAllSubWidgets();
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
			//CCollapsibleSection* section = CreateWidget<CCollapsibleSection>(this, "ObjectEditorSection");
			//section->SetTitle(category);
			{
				for (auto field = targetType->GetFirstField(); field != nullptr; field = field->GetNext())
				{
					if ((category == "General" && !field->HasAttribute("Category")) ||
						(field->HasAttribute("Category") && field->GetAttribute("Category").GetStringValue() == category))
					{
						CLabel* label = CreateWidget<CLabel>(this, "FieldLabel");
						label->SetText(field->GetName().GetString());
					}
				}
			}
		}

		SetNeedsStyle();
		SetNeedsLayout();
	}

	void ObjectEditor::OnDrawGUI()
	{
		Super::OnDrawGUI();

		//if (targets.IsEmpty())
		//	return;

		//auto rect = GetComputedLayoutRect();

		//GUI::BeginChild(rect, GetUuid(), GetName().GetString(), {}, defaultStyleState);
		//DrawDefaultBackground();

		//GUI::PushZeroingChildCoordinateSpace();
		//{
		//	for (auto widget : attachedWidgets)
		//	{
		//		widget->Render();
		//	}
		//}
		//GUI::PopChildCoordinateSpace();

		//GUI::EndChild();
	}

} // namespace CE::Editor
