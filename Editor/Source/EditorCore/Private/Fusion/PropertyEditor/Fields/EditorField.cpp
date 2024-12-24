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

    EditorField::Self& EditorField::BindField(const Ref<Object>& target, const CE::Name& relativeFieldPath)
    {
        if (!CanBind(target, relativeFieldPath))
        {
            return *this;
        }

        this->targets = Array<WeakRef<Object>>{ target };
        this->relativeFieldPath = relativeFieldPath;

        // TODO

        isBound = true;
        OnBind();
        UpdateValue();

        return *this;
    }

    EditorField::Self& EditorField::UnbindField()
    {
        isBound = false;
    	targets.Clear();
        
        return *this;
    }

    EditorField& EditorField::FixedInputWidth(f32 width)
    {
        return *this;
    }

}

