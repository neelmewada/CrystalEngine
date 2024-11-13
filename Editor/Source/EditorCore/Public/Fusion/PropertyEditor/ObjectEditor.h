#pragma once

namespace CE::Editor
{
    class PropertyEditor;

    CLASS()
    class EDITORCORE_API ObjectEditor : public FStyledWidget, IObjectUpdateListener
    {
        CE_CLASS(ObjectEditor, FStyledWidget)
    protected:

        ObjectEditor();

        virtual ~ObjectEditor();

        void Construct() override;

        void OnBeforeDestroy() override;

        void OnObjectFieldChanged(Object* object, const CE::Name& fieldName) override;

    public:

        virtual bool SupportsMultiObjectEditing() const { return false; }

        Self& FixedInputWidth(f32 width);

        f32 GetSplitRatio();

        void SetSplitRatio(f32 ratio, FSplitBox* excluding = nullptr);

        void ApplySplitRatio(FSplitBox* excluding = nullptr);

    protected:

        virtual void CreateGUI();

        FVerticalStack* content = nullptr;

        Array<PropertyEditor*> propertyEditors;

        FIELD(ReadOnly)
        Array<Object*> targets;

        FIELD(ReadOnly)
        Object* target = nullptr;

        friend class ObjectEditorRegistry;
    };
    
} // namespace CE

#include "ObjectEditor.rtti.h"
