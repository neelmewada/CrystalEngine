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
				if (category != "General")
					categories.Add(category);
			}
		}

		return this->targets.NonEmpty();
	}

	Vec2 ObjectEditor::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(0, 0);
	}

	void ObjectEditor::Construct()
	{
		Super::Construct();
	}

	void ObjectEditor::OnDrawGUI()
	{
		if (targets.IsEmpty())
			return;

		auto rect = GetComputedLayoutRect();

		GUI::BeginChild(rect, GetUuid(), "Object", {}, defaultStyleState);
		DrawDefaultBackground();

		GUI::PushZeroingChildCoordinateSpace();
		{
			
		}
		GUI::PopChildCoordinateSpace();

		GUI::EndChild();
	}

} // namespace CE::Editor
