#pragma once

namespace CE::Editor
{
    class PropertyEditor;

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

        Self& FixedInputWidth(f32 width);

        f32 GetSplitRatio();

        void SetSplitRatio(f32 ratio);

    protected:

        void CreateGUI();

        FUNCTION()
        void OnSplitterDragged(FSplitBox* splitBox);

        FVerticalStack* content = nullptr;

        Array<FSplitBox*> splitters;
        Array<PropertyEditor*> propertyEditors;

        FIELD(ReadOnly)
        Array<Object*> targets;

        FIELD(ReadOnly)
        Object* target = nullptr;

        friend class ObjectEditorRegistry;
    };
    
} // namespace CE

#include "ObjectEditor.rtti.h"
