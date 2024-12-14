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

    void EditorField::OnBeginDestroy()
    {
	    Super::OnBeginDestroy();

        UnbindField();
    }

    EditorField::Self& EditorField::BindField(FieldType* field, Object* target, void* instance)
    {
        if (!field || !CanBind(field))
            return *this;

        this->field = field;
        targets = { target };
        instances = { instance };

        OnBind();

        UpdateValue();

        return *this;
    }

    EditorField::Self& EditorField::BindField(FieldType* field, Object* target)
    {
        return BindField(field, target, target);
    }

    EditorField::Self& EditorField::UnbindField()
    {
        field = nullptr;
        targets.Clear();
        instances.Clear();
        return *this;
    }

    EditorField& EditorField::FixedInputWidth(f32 width)
    {
        return *this;
    }

}

