#pragma once

namespace CE
{
    CLASS()
    class EDITORCORE_API ObjectEditor : public FStyledWidget
    {
        CE_CLASS(ObjectEditor, FStyledWidget)
    protected:

        ObjectEditor();

        virtual ~ObjectEditor();

        void Construct() override;

        void OnBeforeDestroy() override;

    public:

        virtual bool SupportsMultiObjectEditing() const { return false; }

    protected:

        void CreateGUI();

        FVerticalStack* content = nullptr;

        FIELD(ReadOnly)
        Array<Object*> targets;

        FIELD(ReadOnly)
        Object* target = nullptr;

        friend class ObjectEditorRegistry;
    };
    
} // namespace CE

#include "ObjectEditor.rtti.h"
