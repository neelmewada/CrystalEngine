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

        
    }

    void ObjectEditor::CreateGUI()
    {
        // Default ObjectEditor doesn't support multi-object editing yet.
        if (target == nullptr)
            return;

        if (FWidget* child = GetChild())
        {
            RemoveChild(child);
            child->Destroy();
        }

        // TODO: Create editors and bind them to respective fields of Object
        // Editor widget will hold reference to FieldType* (edited field) and Array<Object*> (array of target objects)
        // Editor widget will listen to changes made in any of the Array<Object*> through code (NOT UI input) using the Setters.

        Class* clazz = target->GetClass();

        u32 numFields = clazz->GetFieldCount();

        HashMap<CE::Name, Array<FieldType*>> fieldsByCategory;
        Array<CE::Name> categories;
        HashMap<CE::Name, int> categoryOrders;

        for (int i = 0; i < numFields; ++i)
        {
            Field* field = clazz->GetFieldAt(i);

            if (!field->IsEditAnywhere())
                continue;

            CE::Name category = "General";

            if (!field->HasAttribute("Category"))
            {
	            fieldsByCategory["General"].Add(field);
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

            fieldsByCategory[category].Add(field);
        }

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

            for (int j = 0; j < fieldsByCategory[category].GetSize(); ++j)
            {
                FieldType* field = fieldsByCategory[category][j];


            }
        }
    }

} // namespace CE

