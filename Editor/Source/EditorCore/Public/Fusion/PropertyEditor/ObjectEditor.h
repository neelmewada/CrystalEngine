#pragma once

namespace CE
{
    CLASS()
    class EDITORCORE_API ObjectEditor : public Object
    {
        CE_CLASS(ObjectEditor, Object)
    protected:

        ObjectEditor();
        
    public:

        virtual ~ObjectEditor();

        virtual bool SupportsMultiObjectEditing() const { return false; }

    protected:

    private:

        FIELD(ReadOnly)
        Array<Object*> targets;

        FIELD(ReadOnly)
        Object* target = nullptr;

        friend class ObjectEditorRegistry;
    };
    
} // namespace CE

#include "ObjectEditor.rtti.h"
