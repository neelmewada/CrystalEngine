#include "EditorSystem.h"

namespace CE::Editor
{
	HashMap<ClassType*, SubClass<ObjectEditor>> ObjectEditor::customObjectEditors{};

	ObjectEditor::ObjectEditor()
	{
		
	}

	ObjectEditor::~ObjectEditor()
	{
		
	}

	ObjectEditor* ObjectEditor::Create(Class* targetClass, Object* outer, const String& name)
	{
		if (outer == nullptr)
			outer = GetTransient(MODULE_NAME);
		if (targetClass == nullptr)
			return nullptr;

		SubClass<ObjectEditor> editorClass = nullptr;

		Class* parentClass = targetClass;

		while (editorClass == nullptr && parentClass != nullptr)
		{
			if (customObjectEditors.KeyExists(parentClass))
				editorClass = customObjectEditors[parentClass];

			if (parentClass->GetSuperClassCount() == 0)
				break;

			parentClass = parentClass->GetSuperClass(0);
		}

		if (editorClass == nullptr)
			editorClass = GetStaticClass<ObjectEditor>();

		ObjectEditor* editor = CreateObject<ObjectEditor>(outer, name.IsEmpty() ? editorClass->GetName().GetLastComponent() : name, OF_NoFlags, editorClass);
		editor->targetClass = targetClass;
		return editor;
	}

	ObjectEditor* ObjectEditor::Create(Object* target, Object* outer, const String& name)
	{
		if (!target)
			return nullptr;

		ObjectEditor* editor = Create(target->GetClass(), outer, name);
		editor->SetTarget(target);
		return editor;
	}

	void ObjectEditor::SetTarget(Object* target)
	{
		targets.Clear();
		targets.Add(target);
	}

	void ObjectEditor::SetTargets(const Array<Object*>& targets)
	{
		this->targets = targets;
	}

	bool ObjectEditor::CreateGUI(CWidget* parent)
	{
		if (!parent || targets.IsEmpty())
			return false;
		if (targets.GetSize() > 1)
			return false;

		Object* target = targets[0];
		if (!target)
			return false;

		Class* targetClass = target->GetClass();
		HashMap<Name, Array<FieldType*>> fieldsByCategory{};
		Array<Name> categories = { "General" };
		HashMap<Name, int> categoryOrders{};
		categoryOrders["General"] = 0;

		if (target->IsOfType<Actor>())
		{
			Actor* actor = static_cast<Actor*>(target);

			SceneComponent* rootComponent = actor->GetRootComponent();

			if (rootComponent)
			{
				ClassType* componentClass = rootComponent->GetClass();

				for (auto field = componentClass->GetFirstField(); field != nullptr; field = field->GetNext())
				{
					if (field->IsEditAnywhere() || field->IsVisibleAnywhere())
					{
						if (!field->HasAttribute("Category"))
						{
							fieldsByCategory["General"].Add(field);
						}
						else
						{
							Name category = field->GetAttribute("Category").GetStringValue();
							if (!category.IsValid())
							{
								fieldsByCategory["General"].Add(field);
							}
							else
							{
								fieldsByCategory[category].Add(field);
								if (!categories.Exists(category))
									categories.Add(category);
								if (field->HasAttribute("CategoryOrder"))
								{
									String orderString = field->GetAttribute("CategoryOrder");
									int order = 0;
									if (String::TryParse(orderString, order))
									{
										categoryOrders[category] = order;
									}
								}
							}
						}
					}
				}
			}
		}

		for (auto field = targetClass->GetFirstField(); field != nullptr; field = field->GetNext())
		{
			if (field->IsEditAnywhere() || field->IsVisibleAnywhere())
			{
				if (!field->HasAttribute("Category"))
				{
					fieldsByCategory["General"].Add(field);
				}
				else
				{
					Name category = field->GetAttribute("Category").GetStringValue();
					if (!category.IsValid())
					{
						fieldsByCategory["General"].Add(field);
					}
					else
					{
						fieldsByCategory[category].Add(field);
						if (!categories.Exists(category))
							categories.Add(category);
						if (field->HasAttribute("CategoryOrder"))
						{
							String orderString = field->GetAttribute("CategoryOrder");
							int order = 0;
							if (String::TryParse(orderString, order))
							{
								categoryOrders[category] = order;
							}
						}
					}
				}
			}
		}

		categories.Sort([&](const Name& a, const Name& b)
			{
				return categoryOrders[a] < categoryOrders[b];
			});

		propertyDrawers.Clear();

		for (int i = 0; i < categories.GetSize(); ++i)
		{
			if (fieldsByCategory[categories[i]].IsEmpty())
				continue;

			PropertyEditorSection* section = CreateObject<PropertyEditorSection>(parent, "PropertyEditorSection");
			section->SetExpanded(false);
			section->SetTitle(categories[i].GetString());

			const auto& fields = fieldsByCategory[categories[i]];

			CWidget* container = section->GetContent();

			for (FieldType* field : fields)
			{
				PropertyDrawer* propertyDrawer = PropertyDrawer::Create(this, field, container);
				if (!propertyDrawer)
					continue;

				if (((ClassType*)field->GetInstanceOwnerType())->IsSubclassOf<ActorComponent>())
				{
					if (target->IsOfType<Actor>())
					{
						Actor* actor = static_cast<Actor*>(target);
						SceneComponent* rootComponent = actor->GetRootComponent();

						if (rootComponent)
						{
							propertyDrawer->CreateGUI(field, rootComponent);
							propertyDrawers.Add(propertyDrawer);
						}
					}
					else
					{
						propertyDrawer->CreateGUI(field, target);
						propertyDrawers.Add(propertyDrawer);
					}
				}
				else
				{
					propertyDrawer->CreateGUI(field, target);
					propertyDrawers.Add(propertyDrawer);
				}
			}

			section->SetExpanded(true);
		}
		
		parent->SetNeedsLayout();
		parent->SetNeedsPaint();

		return true;
	}

} // namespace CE::Editor
