#include "EditorCore.h"

namespace CE
{

    ObjectEditor::ObjectEditor()
    {

    }

    ObjectEditor::~ObjectEditor()
    {
        
    }

    void ObjectEditor::Construct()
    {
	    Super::Construct();

        Child(
            FAssignNew(FVerticalStack, content)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
        );
        
    }

    void ObjectEditor::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        for (Object* target : targets)
        {
            if (ObjectEditorRegistry::Get().objectEditorsByInstances[target->GetUuid()] == this)
            {
	            ObjectEditorRegistry::Get().objectEditorsByInstances.Remove(target->GetUuid());
            }
        }
    }

    void ObjectEditor::CreateGUI()
    {
        // Default ObjectEditor doesn't support multi-object editing yet.
        if (target == nullptr)
            return;
        
        content->DestroyAllChildren();
        splitters.Clear();

        // TODO: Create editors and bind them to respective fields of Object
        // Editor widget will hold reference to FieldType* (edited field) and Array<Object*> (array of target objects)
        // Editor widget will listen to changes made in any of the Array<Object*> through code (NOT UI input) using the Setters.

        struct FieldData
        {
            FieldType* field = nullptr;
            Object* targetObject = nullptr;
        };

        HashMap<CE::Name, Array<FieldData>> fieldsByCategory;
        Array<CE::Name> categories;
        HashMap<CE::Name, int> categoryOrders;
        HashSet<TypeId> visitedComponents{};

        std::function<void(Object*,bool)> visitor = [&](Object* target, bool recurseComponents)
            {
                Class* clazz = target->GetClass();

                u32 numFields = clazz->GetFieldCount();

                for (int i = 0; i < numFields; ++i)
                {
                    Field* field = clazz->GetFieldAt(i);

                    if (!field->IsEditAnywhere())
                        continue;

                    CE::Name category = "General";

                    if (!field->HasAttribute("Category"))
                    {
                        fieldsByCategory["General"].Add({ field, target });
                    }
                    else
                    {
                        category = field->GetAttribute("Category").GetStringValue();
                        if (!category.IsValid())
                        {
                            category = "General";
                        }
                    }

                    if (!fieldsByCategory.KeyExists(category))
                    {
                        categories.Add(category);
                    }

                    if (field->HasAttribute("CategoryOrder"))
                    {
                        int order = 0;
                        String::TryParse(field->GetAttribute("CategoryOrder").GetStringValue(), order);
                        categoryOrders[category] = order;
                    }

                    fieldsByCategory[category].Add({ field, target });
                }
                
	            if (target->IsOfType<Actor>())
	            {
                    Actor* actor = static_cast<Actor*>(target);

                    if (actor->GetRootComponent())
                    {
                        visitor(actor->GetRootComponent(), true);
                    }
	            }
            };

        visitor(target, target->IsOfType<Actor>());

        categories.Sort([&](const CE::Name& lhs, const CE::Name& rhs)
            {
                int lhsOrder = categoryOrders[lhs];
                int rhsOrder = categoryOrders[rhs];
                if (lhsOrder < rhsOrder)
                    return true;
                if (lhsOrder == rhsOrder)
                    return String::NaturalCompare(lhs.GetString(), rhs.GetString());
                return false;
            });

        for (int i = 0; i < categories.GetSize(); ++i)
        {
            const CE::Name& category = categories[i];
            if (fieldsByCategory[category].IsEmpty())
                continue;

            FExpandableSection* section = nullptr;
            FVerticalStack* expandContent = nullptr;

            FAssignNew(FExpandableSection, section)
                .Title(category.GetString())
				.ExpandableContent(
                    FAssignNew(FVerticalStack, expandContent)
                )
            ;

            content->AddChild(section);

            for (int j = 0; j < fieldsByCategory[category].GetSize(); ++j)
            {
                FieldType* field = fieldsByCategory[category][j].field;
                Object* target = fieldsByCategory[category][j].targetObject;

                if (!field || !target)
                    continue;

                PropertyEditor* propertyEditor = PropertyEditorRegistry::Get().Create(field, target);

            	splitters.Add(propertyEditor->GetSplitBox());
                splitters.Top()->OnSplitterDragged(FUNCTION_BINDING(this, OnSplitterDragged));
                
                expandContent->AddChild(propertyEditor);

                thread_local Array targetsArray = { target };
                targetsArray[0] = target;

                propertyEditor->SetTarget(field, targetsArray);
            }
        }
    }

    void ObjectEditor::OnSplitterDragged(FSplitBox* splitBox)
    {
        f32 splitRatio = splitBox->GetChild(0)->FillRatio();

        for (FSplitBox* splitter : splitters)
        {
	        if (splitter != splitBox)
	        {
                splitter->GetChild(0)->FillRatio(splitRatio);
                splitter->GetChild(1)->FillRatio(1.0f - splitRatio);
	        }
        }
    }

} // namespace CE

