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

        void OnBeginDestroy() override;

        void OnObjectFieldChanged(Object* object, const CE::Name& fieldName) override;

        void SetSplitRatioInternal(f32 ratio, FSplitBox* excluding = nullptr);

    public:

        void SetEditorGroup(const Array<ObjectEditor*>& group);

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

        Array<ObjectEditor*> editorGroup;

    public:

        friend class ObjectEditorRegistry;
    };
    
} // namespace CE

#include "ObjectEditor.rtti.h"
