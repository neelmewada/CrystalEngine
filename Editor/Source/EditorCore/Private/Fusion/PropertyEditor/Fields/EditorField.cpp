#include "EditorCore.h"

namespace CE::Editor
{

    EditorField::EditorField()
    {
        m_HAlign = HAlign::Left;
        m_VAlign = VAlign::Center;
    }

    void EditorField::Construct()
    {
        Super::Construct();
        
    }

    EditorField::Self& EditorField::BindField(FieldType* field, Object* target)
    {
        if (!CanBind(field))
            return *this;

        if (targets.NonEmpty())
        {
            ObjectListener::RemoveListener(targets[0], this);
        }

        this->field = field;
        targets = { target };

        ObjectListener::AddListener(targets[0], this);

        UpdateValue();

        return *this;
    }

    EditorField::Self& EditorField::UnbindField()
    {
        if (targets.NonEmpty())
        {
            ObjectListener::RemoveListener(targets[0], this);
        }

        field = nullptr;
        targets.Clear();
        return *this;
    }

    EditorField& EditorField::FixedInputWidth(f32 width)
    {
        return *this;
    }

    void EditorField::OnObjectFieldChanged(Object* object, const CE::Name& fieldName)
    {
        if (!IsBound())
            return;

        if (object == targets[0])
        {
            UpdateValue();
        }
    }

}

